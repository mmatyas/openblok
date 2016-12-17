#pragma once

#include "game/states/substates/Multiplayer.h"

#include <array>
#include <memory>

class Texture;


namespace SubStates {
namespace Multiplayer {
namespace States {

class PlayerSelect : public State {
public:
    PlayerSelect(AppContext&);
    void update(MultiplayerState&, const std::vector<Event>&, AppContext&) final;
    void draw(MultiplayerState&, GraphicsContext&) const final;

private:
    std::vector<DeviceID> devices;
    std::array<std::unique_ptr<Texture>, 4> tex_player;
    std::unique_ptr<Texture> tex_ok;
    std::unique_ptr<Texture> tex_pending;

    void drawJoinedWell(GraphicsContext&, int x, int y, uint8_t player_id) const;
    void drawPendingWell(GraphicsContext&, int x, int y) const;
    void drawWellBackground(GraphicsContext&, int x, int y) const;
};

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
