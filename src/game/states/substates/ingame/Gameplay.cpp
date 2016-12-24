#include "Gameplay.h"

#include "Countdown.h"
#include "Pause.h"
#include "Statistics.h"
#include "game/AppContext.h"
#include "game/BattleAttackTable.h"
#include "game/components/HoldQueue.h"
#include "game/components/NextQueue.h"
#include "game/components/Piece.h"
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

Gameplay::Gameplay(AppContext& app, IngameState& parent,
                   const std::vector<DeviceID>& player_devices,
                   unsigned short starting_gravity_level)
    : player_devices(player_devices)
    , music(app.audio().loadMusic(Paths::data() + "music/gameplay.ogg"))
    , sfx_onhold(app.audio().loadSound(Paths::data() + "sfx/hold.ogg"))
    , sfx_onlevelup(app.audio().loadSound(Paths::data() + "sfx/levelup.ogg"))
    , sfx_onlineclear({{
            app.audio().loadSound(Paths::data() + "sfx/lineclear1.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/lineclear2.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/lineclear3.ogg"),
            app.audio().loadSound(Paths::data() + "sfx/lineclear4.ogg"),
        }})
    , sfx_onlock(app.audio().loadSound(Paths::data() + "sfx/lock.ogg"))
    , sfx_onrotate(app.audio().loadSound(Paths::data() + "sfx/rotate.ogg"))
    , texts_need_update(true)
    , sfx_ongameover(app.audio().loadSound(Paths::data() + "sfx/gameover.ogg"))
    , sfx_onfinish(app.audio().loadSound(Paths::data() + "sfx/finish.ogg"))
//    , font_popuptext(app.gcx().loadFont(Paths::data() + "fonts/PTS76F.ttf", 34))
//    , pending_levelup_msg(std::chrono::milliseconds(500), [](double){}, [this](){
//            this->textpopups.emplace_back(std::make_unique<TextPopup>(tr("LEVEL UP!"), font_popuptext));
//        })
    , lineclears_per_level(10)
    , gameend_statistics_delay(std::chrono::seconds(5),
        [](double t){ return t * 5; },
        [&parent, &app](){
            parent.states.emplace_back(std::make_unique<Statistics>(parent, app));
        })
{
    assert(player_devices.size() > 0);
    assert(player_devices.size() <= 4);
    assert(starting_gravity_level < 15);
    assert(parent.player_areas.empty());


    const bool is_battle = (parent.gamemode == GameMode::MP_BATTLE);

    for (const DeviceID device_id : player_devices) {
        lineclears_left[device_id] = lineclears_per_level;
        previous_lineclear_type[device_id] = ScoreType::CLEAR_SINGLE;
        back2back_length[device_id] = 0;
        pending_garbage_lines[device_id] = 0;

        player_status[device_id] = PlayerStatus::PLAYING;
        parent.player_areas.emplace(std::piecewise_construct,
                std::forward_as_tuple(device_id), std::forward_as_tuple(app, is_battle));
        parent.player_stats.emplace(std::piecewise_construct,
            std::forward_as_tuple(device_id), std::forward_as_tuple());
    }
    {
        // TODO: consider alternative algorithm
        auto& gravity_stack = gravity_levels[player_devices.front()];
        for (int i = 14; i >= starting_gravity_level; i--) {
            float multiplier = std::pow(0.8 - (i * 0.007), i);
            gravity_stack.push(std::chrono::duration_cast<Duration>(multiplier * std::chrono::seconds(1)));
        }

        for (const DeviceID device_id : player_devices) {
            gravity_levels[device_id] = gravity_stack;
            parent.player_areas.at(device_id).well().setGravity(gravity_stack.top());
        }
    }


    parent.updatePositions(app.gcx());

    auto font_big = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 45);
    tex_finish = font_big->renderText(tr("YOU WIN!"), 0xEEEEEE_rgb);

    music->playLoop();
    app.audio().pauseAll();

    gameend_statistics_delay.stop();

    registerObservers(parent, app);
}


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


void Gameplay::registerObservers(IngameState& parent, AppContext& app)
{
    for (const DeviceID device_id : player_devices) {
        auto& well = parent.player_areas.at(device_id).well();

        well.registerObserver(WellEvent::Type::PIECE_LOCKED, [this, &parent, device_id](const WellEvent&){
            sfx_onlock->playOnce();
            auto& parea = parent.player_areas.at(device_id);
            pending_garbage_lines.at(device_id) = parea.queuedGarbageLines();
            parea.setGarbageCount(0);
        });

        well.registerObserver(WellEvent::Type::PIECE_ROTATED, [this](const WellEvent&){
            sfx_onrotate->playOnce();
        });

        well.registerObserver(WellEvent::Type::NEXT_REQUESTED, [this, &parent, device_id](const WellEvent&){
            // if the game is still running
            if (player_status.at(device_id) == PlayerStatus::PLAYING)
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

            texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);


            // increase score

            const auto score_type = ScoreTable::lineclearType(event.lineclear);
            unsigned score = ScoreTable::value(score_type);
            std::string popup_text = ScoreTable::name(score_type);
            player_stats.event_count[score_type]++;
            player_stats.total_cleared_lines += event.lineclear.count;

            const bool back2back = ScoreTable::canContinueBackToBack(previous_lineclear_type.at(device_id), score_type);
            if (back2back) {
                score *= ScoreTable::back2backMultiplier();
                popup_text = ScoreTable::back2backName() + "\n" + popup_text;
                back2back_length.at(device_id)++;
                player_stats.back_to_back_count++;
                player_stats.back_to_back_longest = std::max(player_stats.back_to_back_longest,
                                                            back2back_length.at(device_id));
            }
            else
                back2back_length.at(device_id) = 0;

            player_stats.score += score * player_stats.level;
            previous_lineclear_type.at(device_id) = score_type;


            // send garbage lines to other players, if the game mode allows it

            if (parent.gamemode == GameMode::MP_BATTLE) {
                unsigned sendable_lines = BattleAttackTable::sendableLineCount(event.lineclear, back2back);
                if (sendable_lines > 0) {
                    // reduce current garbage
                    auto& parea = parent.player_areas.at(device_id);
                    unsigned current_queue = parea.queuedGarbageLines();
                    const unsigned smallest = std::min(sendable_lines, current_queue);
                    current_queue -= smallest;
                    sendable_lines -= smallest;
                    parea.setGarbageCount(current_queue);
                    pending_garbage_lines.at(device_id) = current_queue;
                }
                // if we can still send some lines
                if (sendable_lines > 0) {
                    // find target player
                    std::vector<DeviceID> possible_players;
                    for (const DeviceID possible_device : player_devices) {
                        if (player_status.at(possible_device) == PlayerStatus::PLAYING && possible_device != device_id)
                            possible_players.push_back(possible_device);
                    }
                    assert(!possible_players.empty());
                    std::random_shuffle(possible_players.begin(), possible_players.end());
                    DeviceID target_id = possible_players.front();
                    assert(target_id != device_id);

                    // send the lines
                    auto& target_player = parent.player_areas.at(target_id);
                    target_player.setGarbageCount(target_player.queuedGarbageLines() + sendable_lines);
                    // TODO: visual effect
                }
            }


            // increase gravity level

            auto& lines_left = lineclears_left.at(device_id);
            lineclears_left.at(device_id) -= event.lineclear.count;

            if (lines_left <= 0) {
                auto& gravity_stack = gravity_levels.at(device_id);
                assert(!gravity_stack.empty());
                gravity_stack.pop();

                if (gravity_stack.empty()) {
                    lines_left = 0;
                    // IF MARATHON/finishable
                    if (parent.gamemode == GameMode::SP_MARATHON || parent.gamemode == GameMode::MP_MARATHON) {
                        player_status.at(device_id) = PlayerStatus::FINISHED;
                        gameend_anim_timers.at(device_id).restart();
                        sfx_onfinish->playOnce();
                        gameend_statistics_delay.restart();

                        // find out who else is still playing
                        if (playingPlayers().empty())
                            music->fadeOut(std::chrono::seconds(1));
                    }
                    // IF BATTLE
                        // wait until someone gets game over
                    return;
                }
                parent.player_areas.at(device_id).well().setGravity(gravity_stack.top());
                lines_left += lineclears_per_level;
                player_stats.level++;
                sfx_onlevelup->playOnce();

//                // produce delayed popup if there are other popups already
//                pending_levelup_msg.restart();
//                if (textpopups.empty())
//                    pending_levelup_msg.update(this->pending_levelup_msg.length());
            }
        });

        well.registerObserver(WellEvent::Type::MINI_TSPIN_DETECTED, [this, &parent, device_id](const WellEvent&){
            this->texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += ScoreTable::value(ScoreType::MINI_TSPIN);
            player_stats.event_count[ScoreType::MINI_TSPIN]++;
//            this->textpopups.emplace_back(std::make_unique<TextPopup>(
//                ScoreTable::name(ScoreType::MINI_TSPIN),
//                this->font_popuptext));
        });

        well.registerObserver(WellEvent::Type::TSPIN_DETECTED, [this, &parent, device_id](const WellEvent&){
            this->texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += ScoreTable::value(ScoreType::TSPIN);
            player_stats.event_count[ScoreType::TSPIN]++;
//            this->textpopups.emplace_back(std::make_unique<TextPopup>(
//                ScoreTable::name(ScoreType::TSPIN),
//                this->font_popuptext));
        });

        well.registerObserver(WellEvent::Type::HARDDROPPED, [this, &parent, device_id](const WellEvent& event){
            assert(event.harddrop.count < 22);
            this->texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += event.harddrop.count * ScoreTable::value(ScoreType::HARDDROP);
        });

        well.registerObserver(WellEvent::Type::SOFTDROPPED, [this, &parent, device_id](const WellEvent&){
            this->texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += ScoreTable::value(ScoreType::SOFTDROP);
        });

        well.registerObserver(WellEvent::Type::GAME_OVER, [this, &parent, &app, device_id](const WellEvent&){
            // set game over for the triggering player
            player_status.at(device_id) = PlayerStatus::GAME_OVER;
            gameend_anim_timers.at(device_id).restart();

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
                    gameend_anim_timers.at(pdevid).restart();
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

//        auto& pending_msg = pending_levelup_msg.at(device_id);
//        if (pending_msg) {
//            pending_msg.update(Timing::frame_duration);
//        }

//        // newly created popups don't know theit position,
//        // that's why this is here, and not in SinglePlayState
//        const int center_x = parea.x() - 10
//            + (parea.wellBox().x - 10 - parea.x()) / 2;
//        for (auto& popup : textpopups) {
//            popup->setInitialPosition(
//                center_x - static_cast<int>(popup->width()) / 2,
//                parea.y() + parea.height() / 2
//            );
//            popup->update();
//        }
    }

    // remove old animations
//    textpopups.erase(std::remove_if(textpopups.begin(), textpopups.end(),
//        [](std::unique_ptr<TextPopup>& popup){ return !popup->isActive(); }),
//        textpopups.end());
}

void Gameplay::update(IngameState& parent, const std::vector<Event>& events, AppContext& app)
{
    const bool someone_still_playing = !playingPlayers().empty();
    std::map<DeviceID, std::vector<InputEvent>> input_events;

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

    for (const DeviceID device_id : player_devices) {
        if (player_status.at(device_id) == PlayerStatus::PLAYING) {
            auto& well = parent.player_areas.at(device_id).well();

            well.updateGameplayOnly(input_events[device_id]);
            well.addGarbageLines(pending_garbage_lines.at(device_id));
            pending_garbage_lines.at(device_id) = 0;

            parent.player_stats.at(device_id).gametime += Timing::frame_duration;
        }
        parent.player_areas.at(device_id).update();

        // gameend_anim_timers.at(device_id).update(Timing::frame_duration);
    }

    if (texts_need_update) {
        for (const DeviceID device_id : player_devices) {
            const auto& stats = parent.player_stats.at(device_id);
            auto& parea = parent.player_areas.at(device_id);
            parea.setGoalCounter(lineclears_left.at(device_id));
            parea.setLevelCounter(stats.level);
            parea.setScore(stats.score);
            parea.setGametime(stats.gametime);
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
}

void Gameplay::drawActive(IngameState& parent, GraphicsContext& gcx) const
{
   for (const auto& parea : parent.player_areas)
        parea.second.drawActive(gcx);

   for (const DeviceID device_id : player_devices) {
        const auto& parea = parent.player_areas.at(device_id);
        switch (player_status.at(device_id)) {
            case PlayerStatus::GAME_OVER: {
                const auto& wellbox = parea.wellBox();
                const int box_h = wellbox.h * gameend_anim_timers.at(device_id).value();
                gcx.drawFilledRect({
                    wellbox.x, wellbox.y + wellbox.h - box_h,
                    wellbox.w, box_h
                }, 0xA0_rgba);
            }
            break;
            case PlayerStatus::FINISHED:
                tex_finish->drawAt(parea.wellCenterX() - tex_finish->width() / 2,
                                   parea.wellCenterY() - tex_finish->height() / 2);
            break;
            default:
                break;
        }
    }
}

} // namespace States
} // namespace Ingame
} // namespace SubStates
