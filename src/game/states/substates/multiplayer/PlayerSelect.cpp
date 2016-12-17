#include "PlayerSelect.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "game/components/MinoStorage.h"
#include "game/states/MultiplayerState.h"
#include "system/Color.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"

#include <assert.h>
#include <algorithm>


namespace SubStates {
namespace Multiplayer {
namespace States {

static const int well_width = 10 * Mino::texture_size_px;
static const int well_height = 20 * Mino::texture_size_px;
static const int well_padding_x = Mino::texture_size_px / 2;

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
}

void PlayerSelect::onPlayerJoin(DeviceID device_id)
{
    assert(devices.size() < 4);
    assert(devices.size() == player_ids.size());
    // store the joining player's device id
    devices.push_back(device_id);
    // find the next available "name" (color) for the joining player
    for (uint8_t new_player_id = 0; new_player_id < 4; new_player_id++) {
        const bool player_id_exists = player_ids.end()
            != std::find(player_ids.begin(), player_ids.end(), new_player_id);
        if (!player_id_exists) {
            player_ids.push_back(new_player_id);
            break;
        }
    }
    assert(devices.size() == player_ids.size());
}

void PlayerSelect::onPlayerLeave(DeviceID device_id)
{
    assert(devices.size() == player_ids.size());
    for (size_t idx = 0; idx < devices.size(); idx++) {
        if (devices.at(idx) == device_id) {
            // disconnect the player
            devices.erase(devices.begin() + idx);
            // make the leaving player's color available again
            player_ids.erase(player_ids.begin() + idx);
            break;
        }
    }
    assert(devices.size() == player_ids.size());
}

void PlayerSelect::update(MultiplayerState&, const std::vector<Event>& events, AppContext& app)
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
                    if (devices.size() < 4) {
                        const bool device_exists = devices.end()
                            != std::find(devices.begin(), devices.end(), event.input.srcDeviceID());
                        if (!device_exists)
                            onPlayerJoin(event.input.srcDeviceID());
                    }
                    break;
                case InputType::MENU_CANCEL:
                    if (devices.empty()) {
                        //parent.states.emplace_back(std::make_unique<FadeOut>(app));
                        app.states().pop();
                        return;
                    }
                    onPlayerLeave(event.input.srcDeviceID());
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

void PlayerSelect::draw(MultiplayerState&, GraphicsContext& gcx) const
{
    static const int well_full_width = well_width + 2 * well_padding_x;
    const int well_count = std::min<int>(devices.size() + 1, 4);

    int well_x = (gcx.screenWidth() - well_full_width * well_count) / 2;
    const int well_y = (gcx.screenHeight() - well_height) / 2;

    for (const auto& player_id : player_ids) {
        drawJoinedWell(gcx, well_x, well_y, player_id);
        well_x += well_full_width;
    }
    if (devices.size() < 4)
        drawPendingWell(gcx, well_x, well_y);
}

void PlayerSelect::drawWellBackground(GraphicsContext& gcx, int x, int y) const
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
} // namespace Multiplayer
} // namespace SubStates
