#include "Gameplay.h"

#include "Countdown.h"
#include "Pause.h"
#include "Statistics.h"
#include "game/AppContext.h"
#include "game/components/Piece.h"
#include "game/states/MultiplayerState.h"
#include "system/AudioContext.h"
#include "system/Font.h"
#include "system/Localize.h"
#include "system/Music.h"
#include "system/Paths.h"
#include "system/SoundEffect.h"

#include <algorithm>
#include <cmath>


namespace SubStates {
namespace Multiplayer {
namespace States {

Gameplay::Gameplay(MultiplayerState& parent, AppContext& app, const std::vector<DeviceID>& player_devices)
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
    , lineclears_per_level(10)
    , gameend_statistics_delay(std::chrono::seconds(5),
        [](double t){ return t * 5; },
        [&parent, &app](){ parent.states.emplace_back(std::make_unique<Statistics>(parent, app)); })
{
    assert(player_devices.size() > 1);
    assert(player_devices.size() <= 4);
    {
        auto& gravity_stack = gravity_levels[player_devices.front()];
        // TODO: consider alternative algorithm
        for (int i = 14; i >= 0; i--) {
            float multiplier = std::pow(0.8 - (i * 0.007), i);
            gravity_stack.push(std::chrono::duration_cast<Duration>(multiplier * std::chrono::seconds(1)));
        }
        for (const DeviceID device_id : player_devices)
            gravity_levels[device_id] = gravity_levels[player_devices.front()];

        assert(player_devices.size() == gravity_levels.size());
    }
    for (const DeviceID device_id : player_devices) {
        assert(parent.ui_wells.count(device_id) == 0);

        lineclears_left[device_id] = lineclears_per_level;
        previous_lineclear_type[device_id] = ScoreType::CLEAR_SINGLE;
        back2back_length[device_id] = 0;
        player_status[device_id] = PlayerStatus::PLAYING;
        gameend_anim_timers.emplace(std::piecewise_construct,
            std::forward_as_tuple(device_id),
            std::forward_as_tuple(std::chrono::seconds(2), [](double t){ return t; }));
        gameend_anim_timers.at(device_id).stop();

        parent.ui_wells.emplace(std::piecewise_construct,
            std::forward_as_tuple(device_id), std::forward_as_tuple(app));
        parent.ui_wells.at(device_id).well().setGravity(gravity_levels.at(device_id).top());
        parent.ui_topbars.emplace(std::piecewise_construct,
            std::forward_as_tuple(device_id), std::forward_as_tuple(app));
        parent.ui_bottombars.emplace(std::piecewise_construct,
            std::forward_as_tuple(device_id), std::forward_as_tuple(app));
        parent.player_stats.emplace(std::piecewise_construct,
            std::forward_as_tuple(device_id), std::forward_as_tuple());
    }
    assert(parent.ui_wells.size() > 1);
    parent.updatePositions(app.gcx());

    auto font_big = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 45);
    tex_finish = font_big->renderText(tr("YOU WIN!"), 0xEEEEEE_rgb);

    music->playLoop();
    app.audio().pauseAll();

    gameend_statistics_delay.stop();

    registerObservers(parent, app);
}

void Gameplay::addNextPiece(MultiplayerState& parent, DeviceID device_id)
{
    parent.ui_wells.at(device_id).well().addPiece(parent.ui_topbars.at(device_id).nextQueue().next());
    parent.ui_topbars.at(device_id).holdQueue().onNextTurn();
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

void Gameplay::registerObservers(MultiplayerState& parent, AppContext& app)
{
    for (const DeviceID device_id : player_devices) {
        auto& well = parent.ui_wells.at(device_id).well();

        well.registerObserver(WellEvent::Type::PIECE_LOCKED, [this](const WellEvent&){
            sfx_onlock->playOnce();
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
            auto& well = parent.ui_wells.at(device_id).well();
            auto& hold_queue = parent.ui_topbars.at(device_id).holdQueue();

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

            auto& player_stats = parent.player_stats.at(device_id);

            texts_need_update = true;
            lineclears_left.at(device_id) -= event.lineclear.count;
            player_stats.total_cleared_lines += event.lineclear.count;


            // increase score
            auto score_type = ScoreTable::lineclearType(event.lineclear);
            unsigned score = ScoreTable::value(score_type);
            std::string popup_text = ScoreTable::name(score_type);
            player_stats.event_count[score_type]++;

            if (ScoreTable::canContinueBackToBack(previous_lineclear_type.at(device_id), score_type)) {
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


            // increase gravity level
            auto& lines_left = lineclears_left.at(device_id);
            if (lines_left <= 0) {
                auto& gravity_stack = gravity_levels.at(device_id);
                gravity_stack.pop();
                if (gravity_stack.empty()) {
                    lines_left = 0;
                    // IF MARATHON
                        player_status.at(device_id) = PlayerStatus::FINISHED;
                        gameend_anim_timers.at(device_id).restart();
                        sfx_onfinish->playOnce();
                        gameend_statistics_delay.restart();

                        // find out who else is still playing
                        const auto playing_players = playingPlayers();
                        if (playing_players.empty())
                            music->fadeOut(std::chrono::seconds(1));
                    // IF BATTLE
                        // wait until someone gets game over
                    return;
                }
                parent.ui_wells.at(device_id).well().setGravity(gravity_stack.top());
                lines_left += lineclears_per_level;
                player_stats.level++;
                sfx_onlevelup->playOnce();
            }
        });

        well.registerObserver(WellEvent::Type::MINI_TSPIN_DETECTED, [this, &parent, device_id](const WellEvent&){
            this->texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += ScoreTable::value(ScoreType::MINI_TSPIN);
            player_stats.event_count[ScoreType::MINI_TSPIN]++;
        });

        well.registerObserver(WellEvent::Type::TSPIN_DETECTED, [this, &parent, device_id](const WellEvent&){
            this->texts_need_update = true;
            auto& player_stats = parent.player_stats.at(device_id);
            player_stats.score += ScoreTable::value(ScoreType::TSPIN);
            player_stats.event_count[ScoreType::TSPIN]++;
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

            // IF MARATHON
                // wait until all players finish the game
            // IF BATTLE
            {
                // find out who else is still playing
                const auto playing_players = playingPlayers();
                assert(!playing_players.empty());

                // if there's only one player left, s/he is the winner
                if (playing_players.size() == 1) {
                    const DeviceID pdevid = playing_players.front();
                    player_status.at(pdevid) = PlayerStatus::FINISHED;
                    gameend_anim_timers.at(pdevid).restart();
                    sfx_onfinish->playOnce();
                    gameend_statistics_delay.restart();
                    music->fadeOut(std::chrono::seconds(1));
                }
            }
        });
    } // end of `for`
}

void Gameplay::updateAnimationsOnly(MultiplayerState& parent, AppContext&)
{
    for (auto& ui_well : parent.ui_wells)
        ui_well.second.well().updateAnimationsOnly();
}

void Gameplay::update(MultiplayerState& parent, const std::vector<Event>& events, AppContext& app)
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
            parent.ui_wells.at(device_id).well().updateGameplayOnly(input_events[device_id]);
            parent.player_stats.at(device_id).gametime += Timing::frame_duration;
        }
        parent.ui_topbars.at(device_id).update();

        gameend_anim_timers.at(device_id).update(Timing::frame_duration);
    }

    if (texts_need_update) {
        for (const DeviceID device_id : player_devices) {
            const auto& stats = parent.player_stats.at(device_id);
            auto& ui = parent.ui_bottombars.at(device_id);
            ui.updateLevelCounter(stats.level);
            ui.updateScore(stats.score);
        }
        texts_need_update = false;
    }

    updateAnimationsOnly(parent, app);
}

void Gameplay::drawPassive(MultiplayerState& parent, GraphicsContext& gcx) const
{
    for (const auto& ui_well : parent.ui_wells)
        ui_well.second.drawBase(gcx);
    for (const auto& ui_topbar : parent.ui_topbars)
        ui_topbar.second.draw(gcx);
    for (const auto& ui_bottombar : parent.ui_bottombars)
        ui_bottombar.second.draw(gcx);
}

void Gameplay::drawActive(MultiplayerState& parent, GraphicsContext& gcx) const
{
   for (const auto& ui_well : parent.ui_wells)
        ui_well.second.drawContent(gcx);

   for (const DeviceID device_id : player_devices) {
        const auto& ui = parent.ui_wells.at(device_id);
        switch (player_status.at(device_id)) {
            case PlayerStatus::GAME_OVER: {
                const int box_h = ui.wellHeight() * gameend_anim_timers.at(device_id).value();
                gcx.drawFilledRect({
                    ui.wellX(), ui.wellY() + ui.wellHeight() - box_h,
                    ui.wellWidth(), box_h
                }, 0xA0_rgba);
            }
            break;
            case PlayerStatus::FINISHED:
                tex_finish->drawAt(ui.wellX() + (ui.wellWidth() - tex_finish->width()) / 2,
                                   ui.wellY() + (ui.wellHeight() - tex_finish->height()) / 2);
            break;
            default:
                break;
        }
    }
}

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
