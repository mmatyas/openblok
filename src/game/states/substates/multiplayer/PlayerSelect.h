#pragma once

#include "game/states/substates/Multiplayer.h"

namespace SubStates {
namespace Multiplayer {
namespace States {

class PlayerSelect : public State {
public:
    PlayerSelect();
    void update(MultiplayerState&, const std::vector<Event>&, AppContext&) final;
    void draw(MultiplayerState&, GraphicsContext&) const final;

private:
    std::vector<DeviceID> devices;

    void drawWellPlaceholder(GraphicsContext&, int x, int y) const;
};

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
