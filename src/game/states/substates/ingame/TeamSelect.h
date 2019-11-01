#pragma once

#include "game/states/substates/Ingame.h"
#include "system/Color.h"

#include <array>
#include <map>
#include <memory>
#include <unordered_map>

class Texture;


namespace SubStates {
namespace Ingame {
namespace States {

class TeamSelect : public State {
public:
    TeamSelect(AppContext&);
    void update(IngameState&, const std::vector<Event>&, AppContext&) final;
    void drawPassive(IngameState&, GraphicsContext&) const final;

private:
    static constexpr uint8_t MAX_PLAYERS = 4; // also the number of max teams
    std::vector<std::pair<DeviceID, size_t>> team_players;

    std::array<RGBAColor, MAX_PLAYERS> team_colors;
    RGBAColor column_tile_off_color;
    RGBAColor column_tile_on_color;

    std::array<std::unique_ptr<Texture>, MAX_PLAYERS> tex_player;
    std::unique_ptr<Texture> tex_ok;
    std::unique_ptr<Texture> tex_header;
    std::unique_ptr<Texture> tex_join;
    std::unique_ptr<Texture> tex_begin;

    void onPlayerJoin(DeviceID);
    void onPlayerLeave(DeviceID);
    void onPlayerPrevWell(DeviceID);
    void onPlayerNextWell(DeviceID);

    std::vector<size_t> find_joinable_teams_from(size_t) const;
    decltype(team_players)::iterator find_player(DeviceID);
};

} // namespace States
} // namespace Ingame
} // namespace SubStates
