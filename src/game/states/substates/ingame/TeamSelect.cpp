#include "TeamSelect.h"

#include "Countdown.h"
#include "FadeInOut.h"
#include "Gameplay.h"
#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "game/components/MinoStorage.h"
#include "game/states/IngameState.h"
#include "game/util/CircularModulo.h"
#include "system/Color.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"
#include "system/util/MakeUnique.h"

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>


namespace SubStates {
namespace Ingame {
namespace States {

TeamSelect::TeamSelect(AppContext& app)
{
    auto font_small = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 35);
    auto font_large = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 42);
    auto font_ready = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 40);
    tex_ok = font_ready->renderText(tr("READY!"), app.theme().colors.mainmenu_highlight);
    tex_header = font_large->renderText(tr("TEAM SELECT"), app.theme().colors.mainmenu_highlight);
    tex_join = font_small->renderText(tr("PRESS START TO JOIN!"), app.theme().colors.mainmenu_highlight);
    tex_begin = font_small->renderText(tr("PRESS START TO JOIN, OR AGAIN TO BEGIN!"), app.theme().colors.mainmenu_highlight);
    tex_player = {
        font_small->renderText(tr("PLAYER 1"), app.theme().colors.mainmenu_highlight),
        font_small->renderText(tr("PLAYER 2"), app.theme().colors.mainmenu_highlight),
        font_small->renderText(tr("PLAYER 3"), app.theme().colors.mainmenu_highlight),
        font_small->renderText(tr("PLAYER 4"), app.theme().colors.mainmenu_highlight),
    };
    team_colors = {
        0xEE101060_rgba,
        0x10EE1060_rgba,
        0x00B7EB60_rgba,
        0xEEEE1060_rgba,
    };
    team_players.reserve(MAX_PLAYERS);

    column_tile_on_color = 0xffffff30_rgba;
    column_tile_off_color = 0x00000020_rgba;
}

std::vector<size_t> TeamSelect::find_joinable_teams_from(size_t current_team) const
{
    std::array<size_t, MAX_PLAYERS> player_counts {};
    for (const auto& playerinfo : team_players)
        player_counts[playerinfo.second]++;

    assert(current_team < MAX_PLAYERS);
    assert(player_counts[current_team] > 0);
    player_counts[current_team]--;

    std::vector<size_t> possible_teams;
    for (size_t idx = 0; idx < player_counts.size(); idx++) {
        // don't let everyone to be in the same team
        if (player_counts[idx] + 1 < team_players.size())
            possible_teams.emplace_back(idx);
    }

    possible_teams.shrink_to_fit();
    return possible_teams;
}

std::vector<std::pair<DeviceID, size_t>>::iterator TeamSelect::find_player(DeviceID device_id)
{
    return std::find_if(
        team_players.begin(),
        team_players.end(),
        [device_id](const auto& playerinfo){ return playerinfo.first == device_id; });
}

void TeamSelect::onPlayerJoin(DeviceID device_id)
{
    assert(find_player(device_id) == team_players.end());
    if (team_players.size() == MAX_PLAYERS)
        return;

    std::array<size_t, MAX_PLAYERS> player_counts {};
    for (const auto& playerinfo : team_players)
        player_counts[playerinfo.second]++;

    // find an empty team -- if the player can join, there always must be at least one
    const auto it = std::find(player_counts.cbegin(), player_counts.cend(), 0);
    assert(it != player_counts.cend());
    const size_t team_idx = static_cast<size_t>(std::distance(player_counts.cbegin(), it));

    team_players.emplace_back(std::make_pair(device_id, team_idx));
}

void TeamSelect::onPlayerLeave(DeviceID device_id)
{
    const auto player_it = find_player(device_id);
    if (player_it != team_players.end())
        team_players.erase(player_it);

    if (team_players.size() <= 1)
        return;

    std::array<size_t, MAX_PLAYERS> player_counts {};
    for (const auto& playerinfo : team_players)
        player_counts[playerinfo.second]++;

    const size_t empty_teams = static_cast<size_t>(std::count(player_counts.cbegin(), player_counts.cend(), 0));
    const size_t number_of_teams = player_counts.size() - empty_teams;
    if (number_of_teams == 1) {
        const DeviceID last_player_device = team_players.back().first;
        onPlayerNextWell(last_player_device);
    }
}

void TeamSelect::onPlayerNextWell(DeviceID device_id)
{
    const auto player_it = find_player(device_id);
    if (player_it == team_players.cend())
        return;

    const std::vector<size_t> joinable_teams = find_joinable_teams_from(player_it->second);
    assert(!joinable_teams.empty()); // there's always the player's own slot

    const size_t current_team_idx = player_it->second;
    auto next_slot_it = std::find_if(joinable_teams.cbegin(), joinable_teams.cend(),
        [current_team_idx](const size_t team_idx){ return current_team_idx < team_idx; });
    player_it->second = (next_slot_it != joinable_teams.cend())
        ? *next_slot_it
        : joinable_teams.front();
}

void TeamSelect::onPlayerPrevWell(DeviceID device_id)
{
    const auto player_it = find_player(device_id);
    if (player_it == team_players.cend())
        return;

    const std::vector<size_t> joinable_teams = find_joinable_teams_from(player_it->second);
    assert(!joinable_teams.empty()); // there's always the player's own slot

    const size_t current_team_idx = player_it->second;
    auto prev_slot_it = std::find_if(joinable_teams.crbegin(), joinable_teams.crend(),
        [current_team_idx](const size_t team_idx){ return team_idx < current_team_idx; });
    player_it->second = (prev_slot_it != joinable_teams.crend())
        ? *prev_slot_it
        : joinable_teams.back();
}

void TeamSelect::update(IngameState& parent, const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        switch (event.type) {
        case EventType::DEVICE:
            if (event.device.type == DeviceEventType::DISCONNECTED)
                onPlayerLeave(event.device.device_id);
            break;
        case EventType::INPUT:
            if (event.input.down()) {
                switch (event.input.type()) {
                case InputType::MENU_OK:
                    if (team_players.size() <= MAX_PLAYERS) {
                        const DeviceID device_id = event.input.srcDeviceID();
                        const bool device_exists = find_player(device_id) != team_players.end();
                        if (!device_exists) {
                            onPlayerJoin(event.input.srcDeviceID());
                        }
                        else {
                            assert(parent.device_order.empty());
                            for (const auto& playerinfo : team_players)
                                parent.device_order.emplace_back(playerinfo.first);

                            parent.states.emplace_back(std::make_unique<FadeOut>([this, &parent, &app](){
                                std::unordered_map<DeviceID, size_t> team_setup;
                                for (const auto& playerinfo : team_players)
                                    team_setup[playerinfo.first] = playerinfo.second;

                                parent.states.emplace_back(std::make_unique<Gameplay>(app, parent, 0, std::move(team_setup)));
                                parent.states.emplace_back(std::make_unique<Countdown>(app));
                                parent.states.emplace_back(std::make_unique<FadeIn>([&parent, &app](){
                                    parent.states.pop_back();
                                }));
                                parent.states.pop_front(); // pop teamselect
                                parent.states.pop_front(); // pop fadeout
                            }));
                            return;
                        }
                    }
                    break;
                case InputType::MENU_CANCEL:
                    if (team_players.empty()) {
                        parent.states.emplace_back(std::make_unique<FadeOut>([&app](){
                            app.states().pop();
                        }));
                        return;
                    }
                    onPlayerLeave(event.input.srcDeviceID());
                    break;
                case InputType::MENU_LEFT:
                    onPlayerPrevWell(event.input.srcDeviceID());
                    break;
                case InputType::MENU_RIGHT:
                    onPlayerNextWell(event.input.srcDeviceID());
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
}

void TeamSelect::drawPassive(IngameState& parent, GraphicsContext& gcx) const
{
    const auto screen_height = static_cast<int>(gcx.screenHeight() * parent.draw_inverse_scale);
    const auto center_x = static_cast<int>(gcx.screenWidth() * parent.draw_inverse_scale / 2.f);
    const auto center_y = static_cast<int>(gcx.screenHeight() * parent.draw_inverse_scale / 2.f);

    static constexpr int text_padding = 20;
    tex_header->drawAt(center_x - tex_header->width() / 2, text_padding);
    if (team_players.size() > 1)
        tex_begin->drawAt(center_x - tex_begin->width() / 2, screen_height - text_padding - tex_begin->height());
    else
        tex_join->drawAt(center_x - tex_join->width() / 2, screen_height - text_padding - tex_join->height());


    static constexpr int tile_padding = 15;
    const int column_height = screen_height
        - static_cast<int>(tex_header->height())
        - static_cast<int>(tex_join->height())
        - text_padding * 4;
    const int column_width = static_cast<int>(::ceil(column_height / 3.0));
    const int tile_width = column_width - 2 * tile_padding;
    const int tile_height = (column_height - tile_padding) / 4 - tile_padding;

    static constexpr int column_padding = 20;
    ::Rectangle column_rect {
        center_x - (MAX_PLAYERS / 2) * (column_width + column_padding),
        center_y - column_height / 2,
        column_width,
        column_height,
    };
    ::Rectangle tile_rect { 0, 0, tile_width, tile_height };


    for (uint8_t column = 0; column < MAX_PLAYERS; column++) {
        gcx.drawFilledRect(column_rect, team_colors.at(column));

        tile_rect.x = column_rect.x + tile_padding;
        tile_rect.y = column_rect.y + tile_padding;

        for (size_t player_id = 0; player_id < MAX_PLAYERS; player_id++) {
            if (player_id < team_players.size() && team_players.at(player_id).second == column) {
                gcx.drawFilledRect(tile_rect, column_tile_on_color);
                const auto& player_tex = tex_player.at(player_id);
                player_tex->drawAt(tile_rect.x + (tile_rect.w - player_tex->width()) / 2,
                                   tile_rect.y + (tile_rect.h - player_tex->height()) / 2);
            }
            else {
                gcx.drawFilledRect(tile_rect, column_tile_off_color);
            }

            tile_rect.y += tile_height + tile_padding;
        }

        column_rect.x += column_width + column_padding;
    }
}

} // namespace States
} // namespace Ingame
} // namespace SubStates
