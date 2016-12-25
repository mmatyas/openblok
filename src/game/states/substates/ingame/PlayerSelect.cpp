#include "PlayerSelect.h"

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

#include <assert.h>
#include <algorithm>


namespace SubStates {
namespace Ingame {
namespace States {

static const int well_width = 10 * Mino::texture_size_px;
static const int well_height = 20 * Mino::texture_size_px;
static const int well_padding_x = Mino::texture_size_px;

PlayerSelect::PlayerSelect(AppContext& app)
{
    auto font_smaller = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 30);
    auto font_player = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 45);
    auto font_ready = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 40);
    tex_ok = font_ready->renderText(tr("READY!"), 0xEEEEEE_rgb);
    tex_player.at(0) = font_player->renderText(tr("PLAYER A"), 0xEE1010_rgb);
    tex_player.at(1) = font_player->renderText(tr("PLAYER B"), 0x10EE10_rgb);
    tex_player.at(2) = font_player->renderText(tr("PLAYER C"), 0x00B7EB_rgb);
    tex_player.at(3) = font_player->renderText(tr("PLAYER D"), 0xEEEE10_rgb);
    tex_pending = font_smaller->renderText(tr("PRESS START\nTO JOIN"), 0xEEEEEE_rgb);
    tex_begin = font_ready->renderText(tr("PRESS START TO BEGIN!"), 0xEEEEEE_rgb);
}

void PlayerSelect::onPlayerJoin(DeviceID device_id)
{
    assert(column_slots.size() < 4);
    assert(column_slots.size() == player_colors.size());

    // find the next available color for the joining player
    std::set<uint8_t> occupied_colors;
    for (const auto& color : player_colors)
        occupied_colors.insert(color.second);
    for (uint8_t color = 0; color < MAX_PLAYERS; color++) {
        if (!occupied_colors.count(color)) {
            player_colors[device_id] = color;
            break;
        }
    }
    assert(column_slots.size() + 1 == player_colors.size());

    column_slots[MAX_PLAYERS - 1] = device_id;
    onPlayerNextWell(device_id);
    assert(column_slots.size() == player_colors.size());
}

void PlayerSelect::onPlayerLeave(DeviceID device_id)
{
    player_colors.erase(device_id);
    for (auto& entry : column_slots) {
        if (entry.second == device_id)
            column_slots.erase(entry.first);
    }
}

void PlayerSelect::onPlayerNextWell(DeviceID device_id)
{
    // all slots are occupied
    if (column_slots.size() == MAX_PLAYERS)
        return;

    const uint8_t start_col = columnOfPlayer(device_id);
    assert(start_col < MAX_PLAYERS);
    column_slots.erase(start_col);

    uint8_t col = (start_col + 1) % MAX_PLAYERS;
    while (col != start_col) {
        if (!column_slots.count(col)) {
            column_slots[col] = device_id;
            return;
        }
        col = circularModulo(++col, MAX_PLAYERS);
    }

    assert(false);
}

void PlayerSelect::onPlayerPrevWell(DeviceID device_id)
{
    // all slots are occupied
    if (column_slots.size() == MAX_PLAYERS)
        return;

    const int8_t start_col = columnOfPlayer(device_id);
    assert(start_col < MAX_PLAYERS);
    column_slots.erase(start_col);

    int8_t col = circularModulo(start_col - 1, MAX_PLAYERS);
    while (col != start_col) {
        if (!column_slots.count(col)) {
            column_slots[col] = device_id;
            return;
        }
        col = circularModulo(--col, MAX_PLAYERS);
    }

    assert(false);
}

uint8_t PlayerSelect::columnOfPlayer(DeviceID device_id)
{
    for (auto& entry : column_slots) {
        if (entry.second == device_id)
            return entry.first;
    }
    assert(false);
}

void PlayerSelect::update(IngameState& parent, const std::vector<Event>& events, AppContext& app)
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
                    if (column_slots.size() <= MAX_PLAYERS) {
                        const bool device_exists = player_colors.count(event.input.srcDeviceID());
                        if (!device_exists)
                            onPlayerJoin(event.input.srcDeviceID());
                        else if (column_slots.size() > 1) {
                            assert(parent.device_order.empty());
                            for (const auto& slot : column_slots)
                                parent.device_order.push_back(slot.second);

                            parent.states.emplace_back(std::make_unique<FadeOut>([this, &parent, &app](){
                                parent.states.emplace_back(std::make_unique<Gameplay>(app, parent));
                                parent.states.emplace_back(std::make_unique<Countdown>(app));
                                parent.states.emplace_back(std::make_unique<FadeIn>([&parent, &app](){
                                    parent.states.pop_back();
                                }));
                                parent.states.pop_front(); // pop playerselect
                                parent.states.pop_front(); // pop fadeout
                            }));
                            return;
                        }
                    }
                    break;
                case InputType::MENU_CANCEL:
                    if (column_slots.empty()) {
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

void PlayerSelect::drawPassive(IngameState& parent, GraphicsContext& gcx) const
{
    static const int well_full_width = (well_width + 2 * well_padding_x);
    const int center_y = gcx.screenHeight() * parent.draw_inverse_scale / 2;
    const int bottom_area_height = tex_begin->height() + 20;
    const int well_y = center_y - bottom_area_height / 2 - well_height / 2;
    int well_x = ((gcx.screenWidth() * parent.draw_inverse_scale - well_full_width * MAX_PLAYERS) / 2);

    for (uint8_t column = 0; column < MAX_PLAYERS; column++) {
        if (!column_slots.count(column))
            drawPendingWell(gcx, well_x, well_y);
        else {
            const DeviceID player_device = column_slots.at(column);
            assert(player_colors.count(player_device));
            drawJoinedWell(gcx, well_x, well_y, player_colors.at(player_device));
        }
        well_x += well_full_width;
    }

    if (column_slots.size() > 1) {
        const int center_x = gcx.screenWidth() * parent.draw_inverse_scale / 2;
        tex_begin->drawAt(center_x - tex_begin->width() / 2,
                          well_y + well_height + 20);
    }
}

void PlayerSelect::drawWellBackground(GraphicsContext&, int x, int y) const
{
    const auto& cell = MinoStorage::getMatrixCell();
    for (unsigned row = 0; row < 20; row++) {
        for (unsigned col = 0; col < 10; col++)
            cell->draw(x + col * Mino::texture_size_px, y + row * Mino::texture_size_px);
    }
}

void PlayerSelect::drawJoinedWell(GraphicsContext& gcx, int x, int y, uint8_t player_id) const
{
    assert(player_id < 4);

    x += well_padding_x;
    drawWellBackground(gcx, x, y);

    const int center_y = well_height / 2;
    tex_ok->drawAt(x + (well_width - tex_ok->width()) / 2,
                   y + center_y);

    const auto& player_tex = tex_player.at(player_id);
    player_tex->drawAt(x + (well_width - player_tex->width()) / 2,
                       y + center_y - tex_player.at(player_id)->height());
}

void PlayerSelect::drawPendingWell(GraphicsContext& gcx, int x, int y) const
{
    x += well_padding_x;
    drawWellBackground(gcx, x, y);
    tex_pending->drawAt(
        x + (well_width - static_cast<int>(tex_pending->width())) / 2,
        y + (well_height - static_cast<int>(tex_pending->height())) / 2);
}

} // namespace States
} // namespace Ingame
} // namespace SubStates
