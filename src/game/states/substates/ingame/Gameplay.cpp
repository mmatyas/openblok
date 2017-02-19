#include "Gameplay.h"

#include "Countdown.h"
#include "Pause.h"
#include "Statistics.h"
#include "game/AppContext.h"
#include "game/BattleAttackTable.h"
#include "game/components/HoldQueue.h"
#include "game/components/NextQueue.h"
#include "game/components/Piece.h"
#include "game/components/animations/TextPopup.h"
#include "game/states/IngameState.h"
#include "system/AudioContext.h"
#include "system/Font.h"
#include "system/Localize.h"
#include "system/Music.h"
#include "system/Paths.h"
#include "system/SoundEffect.h"

#include <algorithm>
#include <cmath>


namespace SubStates {
namespace Ingame {
namespace States {

Gameplay::Gameplay(AppContext& app, IngameState& parent, unsigned short starting_gravity_level)
    : player_devices(parent.device_order)
    , theme_settings(app.theme().gameplay)
    , music(app.audio().loadMusic(app.theme().random_game_music()))
    , font_popuptext(app.gcx().loadFont(Paths::data() + "fonts/PTS76F.ttf", 34))
    , sfx_onhold(app.audio().loadSound(app.theme().get_sfx("hold.ogg")))
    , sfx_onlevelup(app.audio().loadSound(app.theme().get_sfx("levelup.ogg")))
    , sfx_onlineclear({{
            app.audio().loadSound(app.theme().get_sfx("lineclear1.ogg")),
            app.audio().loadSound(app.theme().get_sfx("lineclear2.ogg")),
            app.audio().loadSound(app.theme().get_sfx("lineclear3.ogg")),
            app.audio().loadSound(app.theme().get_sfx("lineclear4.ogg")),
        }})
    , sfx_onlock(app.audio().loadSound(app.theme().get_sfx("lock.ogg")))
    , sfx_onrotate(app.audio().loadSound(app.theme().get_sfx("rotate.ogg")))
    , sfx_ongarbageadded(app.audio().loadSound(app.theme().get_sfx("garbage.ogg")))
    , texts_need_update(true)
    , sfx_ongameover(app.audio().loadSound(app.theme().get_sfx("gameover.ogg")))
    , sfx_onfinish(app.audio().loadSound(app.theme().get_sfx("finish.ogg")))
    , gameend_statistics_delay(std::chrono::seconds(5),
        [](double t){ return t * 5; },
        [&parent, &app](){
            parent.states.emplace_back(std::make_unique<Statistics>(parent, app));
        })
{
    TextPopup::text_color = app.theme().colors.popup;

    assert(player_devices.size() > 0);
    assert(player_devices.size() <= 4);
    assert(starting_gravity_level < 15);
    parent.player_areas.clear();
    parent.player_stats.clear();


    const bool is_battle = (parent.gamemode == GameMode::MP_BATTLE);

    for (const DeviceID device_id : player_devices) {
        previous_lineclear_type[device_id] = ScoreType::CLEAR_SINGLE;
        back2back_length[device_id] = 0;
        combo_length[device_id] = 0;
        current_piece_cleared_line[device_id] = false;
        prev_piece_cleared_line[device_id] = false;
        pending_garbage_lines[device_id] = 0;

        player_status[device_id] = PlayerStatus::PLAYING;
        parent.player_areas.emplace(std::piecewise_construct,
                std::forward_as_tuple(device_id), std::forward_as_tuple(app, is_battle));
        parent.player_stats.emplace(std::piecewise_construct,
            std::forward_as_tuple(device_id), std::forward_as_tuple());

        textpopups.emplace(std::piecewise_construct,
            std::forward_as_tuple(device_id), std::forward_as_tuple());
    }
    {
        // TODO: consider alternative algorithm
        std::stack<Duration> gravity_stack;
        for (int i = 14; i >= starting_gravity_level; i--) {
            float multiplier = std::pow(0.8 - (i * 0.007), i);
            gravity_stack.push(std::chrono::duration_cast<Duration>(multiplier * std::chrono::seconds(1)));
        }

        for (const DeviceID device_id : player_devices) {
            gravity_levels[device_id] = gravity_stack;
            parent.player_areas.at(device_id).well().setGravity(gravity_stack.top());
            gravity_levels[device_id].pop();
        }
    }
    {
        std::stack<unsigned short> lineclear_stack;
        if (usesDynamicLineAwards(parent)) {
            for (int i = 15; i > starting_gravity_level; i--)
                lineclear_stack.push(i * 5);
        }
        else {
            if (parent.gamemode == GameMode::SP_40LINES)
                lineclear_stack.push(40);
            else {
                for (int i = 15; i > starting_gravity_level; i--)
                    lineclear_stack.push(10);
            }
        }

        for (const DeviceID device_id : player_devices) {
            lineclears_required[device_id] = lineclear_stack;
            lineclears_left[device_id] = lineclear_stack.top();
            lineclears_required[device_id].pop();
        }
    }

    if (is_battle) {
        for (auto& parea : parent.player_areas)
            parea.second.enableGameOverSFX(false);
    }

    parent.updatePositions(app);

    music->playLoop();
    app.audio().pauseAll();

    gameend_statistics_delay.stop();

    registerObservers(parent, app);
}

Gameplay::~Gameplay() = default;

void Gameplay::addNextPiece(IngameState& parent, DeviceID device_id)
{
    auto& parea = parent.player_areas.at(device_id);
    parea.well().addPiece(parea.nextQueue().next());
    parea.holdQueue().onNextTurn();
}


std::vector<DeviceID> Gameplay::playingPlayers()
{
    std::vector<DeviceID> playing_players;
    for (const DeviceID pdevid : player_devices) {
        if (player_status.at(pdevid) == PlayerStatus::PLAYING)
            playing_players.push_back(pdevid);
    }
    return playing_players;
}

void Gameplay::increaseScoreMaybe(IngameState& parent, DeviceID source_player,
                                  const WellEvent::lineclear_t& lcevent)
{
    const auto score_type = ScoreTable::lineclearType(lcevent);
    std::string popup_text = ScoreTable::name(score_type);

    auto& player_stats = parent.player_stats.at(source_player);
    player_stats.event_count[score_type]++;
    player_stats.total_cleared_lines += lcevent.count;


    unsigned score = ScoreTable::value(score_type);
    const bool back2back = ScoreTable::canContinueBackToBack(previous_lineclear_type.at(source_player), score_type);
    if (back2back) {
        score *= ScoreTable::back2backMultiplier();
        popup_text = ScoreTable::back2backName() + "\n" + popup_text;
        back2back_length.at(source_player)++;
        player_stats.back_to_back_count++;
        player_stats.back_to_back_longest = std::max(player_stats.back_to_back_longest,
                                                    back2back_length.at(source_player));
    }
    else
        back2back_length.at(source_player) = 0;

    if (score_type != ScoreType::CLEAR_SINGLE)
        textpopups.at(source_player).emplace_back(popup_text, font_popuptext);


    auto& combo_count = combo_length.at(source_player);
    if (prev_piece_cleared_line.at(source_player)) {
        combo_count++;
        score += ScoreTable::value(ScoreType::COMBO);
        popup_text = std::to_string(combo_count) + ScoreTable::name(ScoreType::COMBO);
        textpopups.at(source_player).emplace_back(popup_text, font_popuptext);
    }
    else
        combo_count = 0;


    player_stats.score += score * player_stats.level;
}

void Gameplay::sendGarbageMaybe(IngameState& parent, DeviceID source_player,
                                const WellEvent::lineclear_t& lcevent)
{
    if (parent.gamemode != GameMode::MP_BATTLE)
        return;

    const auto score_type = ScoreTable::lineclearType(lcevent);
    const bool back2back = ScoreTable::canContinueBackToBack(previous_lineclear_type.at(source_player), score_type);
    unsigned sendable_lines = BattleAttackTable::sendableLineCount(lcevent, back2back);

    if (sendable_lines > 0) {
        // reduce current garbage
        auto& parea = parent.player_areas.at(source_player);
        unsigned current_queue = parea.queuedGarbageLines();
        const unsigned smallest = std::min(sendable_lines, current_queue);
        current_queue -= smallest;
        sendable_lines -= smallest;
        parea.setGarbageCount(current_queue);
        pending_garbage_lines.at(source_player) = current_queue;
    }

    // if we can still send some lines
    if (sendable_lines > 0) {
        // find target player
        std::vector<DeviceID> possible_players;
        for (const DeviceID possible_device : player_devices) {
            if (player_status.at(possible_device) == PlayerStatus::PLAYING && possible_device != source_player)
                possible_players.push_back(possible_device);
        }
        assert(!possible_players.empty());

        std::random_shuffle(possible_players.begin(), possible_players.end());
        DeviceID target_id = possible_players.front();
        assert(target_id != source_player);

        const auto& src_parea = parent.player_areas.at(source_player);
        const auto& dst_parea = parent.player_areas.at(target_id);
        const int distance = dst_parea.wellCenterX() - src_parea.wellCenterX();
        assert(distance != 0);

        attackanims.emplace_back(
            src_parea.wellCenterX(), distance,
            src_parea.wellBox().y, src_parea.wellBox().y + src_parea.wellBox().h,
            [this, &parent, target_id, sendable_lines](){
                auto& target_player = parent.player_areas.at(target_id);
                target_player.setGarbageCount(target_player.queuedGarbageLines() + sendable_lines);
                sfx_ongarbageadded->playOnce();
            });
    }
}

bool Gameplay::usesDynamicLineAwards(IngameState& parent)
{
    switch (parent.gamemode) {
        case GameMode::SP_40LINES:
        case GameMode::SP_MARATHON_SIMPLE:
        case GameMode::MP_MARATHON_SIMPLE:
            return false;
        default:
            return true;
    }
}

void Gameplay::increaseLevelMaybe(IngameState& parent, DeviceID source_player,
                                  const WellEvent::lineclear_t& lcevent)
{
    auto& lines_left = lineclears_left.at(source_player);
    int line_awards = lcevent.count;
    if (usesDynamicLineAwards(parent)) {
        const auto clear_type = ScoreTable::lineclearType(lcevent);
        line_awards = ScoreTable::lineAwards(clear_type);

        if (ScoreTable::canContinueBackToBack(previous_lineclear_type.at(source_player), clear_type))
            line_awards *= ScoreTable::back2backMultiplier();

        line_awards += combo_length.at(source_player) / 2;
    }
    lines_left -= line_awards;

    while (lines_left <= 0) {
        auto& parea = parent.player_areas.at(source_player);
        auto& gravity_stack = gravity_levels.at(source_player);
        auto& line_req_stack = lineclears_required.at(source_player);

        if (line_req_stack.empty() || gravity_stack.empty()) {
            lines_left = 0;
            const bool finishable = (isSinglePlayer(parent.gamemode)
                || parent.gamemode == GameMode::MP_MARATHON
                || parent.gamemode == GameMode::MP_MARATHON_SIMPLE);
            if (finishable) {
                player_status.at(source_player) = PlayerStatus::FINISHED;
                parea.startGameFinish();
                sfx_onfinish->playOnce();
                gameend_statistics_delay.restart();

                // find out who else is still playing
                if (playingPlayers().empty())
                    music->fadeOut(std::chrono::seconds(1));
            }
            return;
        }

        parea.well().setGravity(gravity_stack.top());
        gravity_stack.pop();
        lines_left += line_req_stack.top();
        line_req_stack.pop();
        parent.player_stats.at(source_player).level++;

        sfx_onlevelup->playOnce();
        textpopups.at(source_player).emplace_back(tr("LEVEL UP!"), font_popuptext);
    }
}

void Gameplay::registerObservers(IngameState& parent, AppContext& app)
{
    for (const DeviceID device_id : player_devices) {
        auto& well = parent.player_areas.at(device_id).well();

        well.registerObserver(WellEvent::Type::PIECE_LOCKED, [this, &parent, device_id](const WellEvent&){
            sfx_onlock->playOnce();

            prev_piece_cleared_line.at(device_id) = current_piece_cleared_line.at(device_id);
            current_piece_cleared_line.at(device_id) = false;
        });

        well.registerObserver(WellEvent::Type::PIECE_ROTATED, [this](const WellEvent&){
            sfx_onrotate->playOnce();
        });

        well.registerObserver(WellEvent::Type::NEXT_REQUESTED, [this, &parent, device_id](const WellEvent&){
            if (player_status.at(device_id) != PlayerStatus::PLAYING)
                return;

            auto& parea = parent.player_areas.at(device_id);
            pending_garbage_lines.at(device_id) = parea.queuedGarbageLines();
            parea.setGarbageCount(0);

            addNextPiece(parent, device_id);
        });

        well.registerObserver(WellEvent::Type::HOLD_REQUESTED, [this, &parent, device_id](const WellEvent&){
            auto& well = parent.player_areas.at(device_id).well();
            auto& hold_queue = parent.player_areas.at(device_id).holdQueue();

            hold_queue.onSwapRequested();
            if (hold_queue.swapAllowed()) {
                auto type = well.activePiece()->type();
                well.deletePiece();
                if (hold_queue.isEmpty()) {
                    hold_queue.swapWithEmpty(type);
                    addNextPiece(parent, device_id);
                }
                else
                    well.addPiece(hold_queue.swapWith(type));

                sfx_onhold->playOnce();
            }
        });

        well.registerObserver(WellEvent::Type::LINE_CLEAR_ANIMATION_START, [this](const WellEvent& event){
            assert(event.type == WellEvent::Type::LINE_CLEAR_ANIMATION_START);
            assert(event.lineclear.count > 0);
            assert(event.lineclear.count <= 4);
            sfx_onlineclear.at(event.lineclear.count - 1)->playOnce();
        });

        well.registerObserver(WellEvent::Type::LINE_CLEAR, [this, &parent, &app, device_id](const WellEvent& event){
            assert(event.type == WellEvent::Type::LINE_CLEAR);
            assert(event.lineclear.count > 0);
            assert(event.lineclear.count <= 4);

            increaseScoreMaybe(parent, device_id, event.lineclear);
            sendGarbageMaybe(parent, device_id, event.lineclear);
            increaseLevelMaybe(parent, device_id, event.lineclear);

            previous_lineclear_type.at(device_id) = ScoreTable::lineclearType(event.lineclear);
            current_piece_cleared_line.at(device_id) = true;
            texts_need_update = true;
        });

        well.registerObserver(WellEvent::Type::MINI_TSPIN_DETECTED, [this, &parent, device_id](const WellEvent&){
            texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += ScoreTable::value(ScoreType::MINI_TSPIN);
            player_stats.event_count[ScoreType::MINI_TSPIN]++;

            textpopups.at(device_id).emplace_back(ScoreTable::name(ScoreType::MINI_TSPIN), font_popuptext);
        });

        well.registerObserver(WellEvent::Type::TSPIN_DETECTED, [this, &parent, device_id](const WellEvent&){
            texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += ScoreTable::value(ScoreType::TSPIN);
            player_stats.event_count[ScoreType::TSPIN]++;

            textpopups.at(device_id).emplace_back(ScoreTable::name(ScoreType::TSPIN), font_popuptext);
        });

        well.registerObserver(WellEvent::Type::HARDDROPPED, [this, &parent, device_id](const WellEvent& event){
            assert(event.harddrop.count < 22);
            texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += event.harddrop.count * ScoreTable::value(ScoreType::HARDDROP);
        });

        well.registerObserver(WellEvent::Type::SOFTDROPPED, [this, &parent, device_id](const WellEvent&){
            texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += ScoreTable::value(ScoreType::SOFTDROP);
        });

        well.registerObserver(WellEvent::Type::GAME_OVER, [this, &parent, &app, device_id](const WellEvent&){
            // set game over for the triggering player
            player_status.at(device_id) = PlayerStatus::GAME_OVER;
            parent.player_areas.at(device_id).startGameOver();

            // find out who else is still playing
            auto playing_players = playingPlayers();

            // IF MARATHON
                // wait until all players finish the game
            // IF BATTLE
            if (parent.gamemode == GameMode::MP_BATTLE) {
                // if there's only one player left, s/he is the winner
                if (playing_players.size() == 1) {
                    const DeviceID pdevid = playing_players.front();
                    player_status.at(pdevid) = PlayerStatus::FINISHED;
                    parent.player_areas.at(pdevid).startGameFinish();
                    sfx_onfinish->playOnce();
                    playing_players.clear();
                }
            }

            // if everyone got KO'd, or someone won the battle, end the game
            if (playing_players.empty()) {
                gameend_statistics_delay.restart();
                music->fadeOut(std::chrono::seconds(1));
            }
        });
    } // end of `for`
}

void Gameplay::updateAnimationsOnly(IngameState& parent, AppContext&)
{
    for (const DeviceID device_id : player_devices) {
        auto& parea = parent.player_areas.at(device_id);
        parea.well().updateAnimationsOnly();

        auto& popups = textpopups.at(device_id);

        // remove old animations
        while (!popups.empty() && !popups.front().isActive())
            popups.pop_front();

        // newly created popups don't know their position
        const int center_x = parea.x() - 10
            + (parea.wellBox().x - 10 - parea.x()) / 2;
        for (auto& popup : popups) {
            popup.setInitialPosition(
                center_x - static_cast<int>(popup.width()) / 2,
                parea.y() + parea.height() / 2
            );
            popup.update();
            if (popup.visibility() > (0.6 * 0xFF))
                break;
        }
    }

    attackanims.remove_if([](BattleAttackAnim& anim){ return !anim.isActive(); });
    for (auto& anim : attackanims)
        anim.update();
}

void Gameplay::update(IngameState& parent, const std::vector<Event>& events, AppContext& app)
{
    const bool someone_still_playing = !playingPlayers().empty();
    std::unordered_map<DeviceID, std::vector<InputEvent>> input_events;

    for (const auto& event : events) {
        switch (event.type) {
            case EventType::WINDOW:
                if (someone_still_playing && event.window == WindowEvent::FOCUS_LOST) {
                    parent.states.emplace_back(std::make_unique<Countdown>(app));
                    parent.states.emplace_back(std::make_unique<Pause>(app));
                    return;
                }
                break;
            case EventType::INPUT:
                if (someone_still_playing) {
                    if (event.input.type() == InputType::GAME_PAUSE && event.input.down()) {
                        parent.states.emplace_back(std::make_unique<Countdown>(app));
                        parent.states.emplace_back(std::make_unique<Pause>(app));
                        return;
                    }
                    input_events[event.input.srcDeviceID()].emplace_back(event.input);
                }
                else if (gameend_statistics_delay.value() > 1) {
                    // allow input only after at least one second has passed
                    gameend_statistics_delay.update(gameend_statistics_delay.length());
                }
                break;
            default:
                break;
        }
    }

    gameend_statistics_delay.update(Timing::frame_duration);

    // if singleplayer, merge all input
    if (player_devices.size() == 1) {
        std::vector<InputEvent> temp = input_events[-1];
        input_events.erase(-1);
        for (const auto& event_vec : input_events)
            temp.insert(temp.end(), event_vec.second.begin(), event_vec.second.end());
        input_events.clear();
        input_events.emplace(-1, std::move(temp));
    }

    for (const DeviceID device_id : player_devices) {
        if (player_status.at(device_id) == PlayerStatus::PLAYING) {
            auto& parea = parent.player_areas.at(device_id);
            auto& well = parea.well();

            well.updateGameplayOnly(input_events[device_id]);
            well.addGarbageLines(pending_garbage_lines.at(device_id));
            pending_garbage_lines.at(device_id) = 0;

            auto& stats = parent.player_stats.at(device_id);
            stats.gametime += Timing::frame_duration;
            parea.setGametime(stats.gametime);
        }
        parent.player_areas.at(device_id).update();
    }

    if (parent.gamemode == GameMode::SP_2MIN && someone_still_playing) {
        assert(parent.device_order.size() == 1);
        const DeviceID device_id = parent.device_order.front();
        const auto gametime = parent.player_stats.at(device_id).gametime;
        if (gametime >= std::chrono::minutes(2)) {
            player_status.at(device_id) = PlayerStatus::FINISHED;
            parent.player_areas.at(device_id).startGameFinish();
            sfx_onfinish->playOnce();
            gameend_statistics_delay.restart();
            music->fadeOut(std::chrono::seconds(1));
        }
    }

    if (texts_need_update) {
        for (const DeviceID device_id : player_devices) {
            const auto& stats = parent.player_stats.at(device_id);
            auto& parea = parent.player_areas.at(device_id);
            parea.setGoalCounter(lineclears_left.at(device_id));
            parea.setLevelCounter(app.theme().gameplay.draw_labels, stats.level);
            parea.setScore(stats.score);
        }
        texts_need_update = false;
    }

    // updating the Well may produce popups, initialized at (0;0),
    // so we update the animations only after that
    updateAnimationsOnly(parent, app);
}

void Gameplay::drawPassive(IngameState& parent, GraphicsContext& gcx) const
{
    for (const auto& parea : parent.player_areas)
        parea.second.drawPassive(gcx);

    for (const auto& popup_vec : textpopups) {
        for (const auto& popup : popup_vec.second) {
            popup.draw();
        }
    }

    for (const auto& anim : attackanims)
        anim.draw();
}

void Gameplay::drawActive(IngameState& parent, GraphicsContext& gcx) const
{
   for (const auto& parea : parent.player_areas)
        parea.second.drawActive(gcx);
}

} // namespace States
} // namespace Ingame
} // namespace SubStates
