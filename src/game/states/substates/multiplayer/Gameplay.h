#pragma once

#include "game/ScoreTable.h"
#include "game/Transition.h"
#include "game/states/substates/Multiplayer.h"

#include <array>
#include <memory>
#include <stack>
#include <unordered_map>

class Font;
class Music;
class SoundEffect;
class Texture;


namespace SubStates {
namespace Multiplayer {
namespace States {

class Gameplay : public State {
public:
    Gameplay(MultiplayerState&, AppContext&, const std::vector<DeviceID>&);
    void update(MultiplayerState&, const std::vector<Event>&, AppContext&) final;
    void draw(MultiplayerState&, GraphicsContext&) const final;

private:
    const std::vector<DeviceID> player_devices;

    std::shared_ptr<Music> music;
    std::shared_ptr<SoundEffect> sfx_onhold;
    std::shared_ptr<SoundEffect> sfx_onlevelup;
    std::array<std::shared_ptr<SoundEffect>, 4> sfx_onlineclear;
    std::shared_ptr<SoundEffect> sfx_onlock;
    std::shared_ptr<SoundEffect> sfx_onrotate;
    bool texts_need_update;

    const int lineclears_per_level;
    std::unordered_map<DeviceID, int> lineclears_left;
    std::unordered_map<DeviceID, std::stack<Duration>> gravity_levels;
    std::unordered_map<DeviceID, ScoreType> previous_lineclear_type;
    std::unordered_map<DeviceID, unsigned short> back2back_length;

    void addNextPiece(MultiplayerState& parent);
    void registerObservers(MultiplayerState& parent, AppContext&);
};

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
