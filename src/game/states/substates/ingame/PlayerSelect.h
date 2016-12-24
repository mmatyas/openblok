#pragma once

#include "game/states/substates/Ingame.h"

#include <array>
#include <memory>

class Texture;


namespace SubStates {
namespace Ingame {
namespace States {

class PlayerSelect : public State {
public:
    PlayerSelect(AppContext&);
    void update(IngameState&, const std::vector<Event>&, AppContext&) final;
    void drawPassive(IngameState&, GraphicsContext&) const final;

private:
    std::vector<DeviceID> devices;
    std::vector<uint8_t> player_ids;
    std::array<std::unique_ptr<Texture>, 4> tex_player;
    std::unique_ptr<Texture> tex_ok;
    std::unique_ptr<Texture> tex_pending;
    std::unique_ptr<Texture> tex_begin;

    void onPlayerJoin(DeviceID);
    void onPlayerLeave(DeviceID);

    void drawJoinedWell(GraphicsContext&, int x, int y, uint8_t player_id) const;
    void drawPendingWell(GraphicsContext&, int x, int y) const;
    void drawWellBackground(GraphicsContext&, int x, int y) const;
};

} // namespace States
} // namespace Ingame
} // namespace SubStates
