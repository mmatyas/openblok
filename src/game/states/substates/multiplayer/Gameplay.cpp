#include "Gameplay.h"

#include "game/AppContext.h"
#include "game/states/MultiplayerState.h"
#include "system/AudioContext.h"
#include "system/Music.h"
#include "system/Paths.h"

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
    , lineclears_per_level(10)
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

        parent.ui_wells.emplace(std::piecewise_construct,
                                std::forward_as_tuple(device_id),
                                std::forward_as_tuple(app));
        parent.ui_wells.at(device_id).well().setGravity(gravity_levels.at(device_id).top());
    }
    assert(parent.ui_wells.size() > 1);
    parent.updatePositions(app.gcx());

    music->playLoop();
    app.audio().pauseAll();
}

void Gameplay::update(MultiplayerState& parent, const std::vector<Event>& events, AppContext& app)
{
    std::map<DeviceID, std::vector<InputEvent>> input_events;
    for (const auto& event : events) {
        switch (event.type) {
            case EventType::WINDOW:
                if (event.window == WindowEvent::FOCUS_LOST) {
                    //parent.states.emplace_back(std::make_unique<Countdown>(app));
                    //parent.states.emplace_back(std::make_unique<Pause>(app));
                    return;
                }
                break;
            case EventType::INPUT:
                if (event.input.type() == InputType::GAME_PAUSE && event.input.down()) {
                    //parent.states.emplace_back(std::make_unique<Countdown>(app));
                    //parent.states.emplace_back(std::make_unique<Pause>(app));
                    return;
                }
                input_events[event.input.srcDeviceID()].emplace_back(event.input);
                break;
            default:
                break;
        }
    }

    for (const DeviceID device_id : player_devices) {
        parent.ui_wells.at(device_id).well().updateGameplayOnly(input_events[device_id]);
        //parent.ui_leftside.update();

        //parent.player_stats.at(i).gametime += Timing::frame_duration;
        // parent.ui_rightside.updateGametime(parent.player_stats.gametime);
    }

    if (texts_need_update) {
        for (const DeviceID device_id : player_devices) {
            //parent.ui_leftside.updateGoalCounter(lineclears_left);
            //parent.ui_leftside.updateLevelCounter(parent.player_stats.level);
            //parent.ui_rightside.updateScore(parent.player_stats.score);
        }
        texts_need_update = false;
    }

    updateAnimationsOnly(parent, app);
}

void Gameplay::draw(MultiplayerState& parent, GraphicsContext& gcx) const
{
    for (auto& ui_well : parent.ui_wells) {
        ui_well.second.drawBase(gcx);
        ui_well.second.drawContent(gcx);
    }
}

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
