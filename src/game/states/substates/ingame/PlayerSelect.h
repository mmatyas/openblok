#pragma once

#include "game/states/substates/Ingame.h"

#include <array>
#include <map>
#include <memory>
#include <unordered_map>

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
    static constexpr uint8_t MAX_PLAYERS = 4;
    std::map<uint8_t, DeviceID> column_slots;
    std::unordered_map<DeviceID, uint8_t> player_colors;

    std::array<std::unique_ptr<Texture>, MAX_PLAYERS> tex_player;
    std::unique_ptr<Texture> tex_ok;
    std::unique_ptr<Texture> tex_pending;
    std::unique_ptr<Texture> tex_begin;

    void onPlayerJoin(DeviceID);
    void onPlayerLeave(DeviceID);

    void onPlayerPrevWell(DeviceID);
    void onPlayerNextWell(DeviceID);
    uint8_t columnOfPlayer(DeviceID);

    void drawJoinedWell(GraphicsContext&, int x, int y, uint8_t player_id) const;
    void drawPendingWell(GraphicsContext&, int x, int y) const;
    void drawWellBackground(GraphicsContext&, int x, int y) const;
};

} // namespace States
} // namespace Ingame
} // namespace SubStates
