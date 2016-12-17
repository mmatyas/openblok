#include "PlayerSelect.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "game/components/MinoStorage.h"
#include "system/Color.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"

#include <assert.h>


namespace SubStates {
namespace Multiplayer {
namespace States {

static const int well_width = 10 * Mino::texture_size_px;
static const int well_height = 20 * Mino::texture_size_px;
static const int well_padding_x = Mino::texture_size_px;

PlayerSelect::PlayerSelect(AppContext& app)
{
    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 30);
    tex_ok = font->renderText(tr("READY!"), 0xEEEEEE_rgb);
    tex_pending = font->renderText(tr("PRESS START\nTO JOIN"), 0xEEEEEE_rgb);
}

void PlayerSelect::update(MultiplayerState&, const std::vector<Event>&, AppContext&)
{

}

void PlayerSelect::draw(MultiplayerState&, GraphicsContext& gcx) const
{
    static const int well_full_width = well_width + 2 * well_padding_x;
    const int well_count = std::min<int>(devices.size() + 1, 4);

    int well_x = (gcx.screenWidth() - well_full_width * well_count) / 2;
    const int well_y = (gcx.screenHeight() - well_height) / 2;
    for (const auto& device_id : devices) {
        drawJoinedWell(gcx, well_x, well_y);
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

void PlayerSelect::drawJoinedWell(GraphicsContext& gcx, int x, int y) const
{
    /*static const auto color = 0x1A3A8A_rgba;
    const Rectangle box = {x + placeholder_padding, y, placeholder_width, placeholder_height};
    gcx.drawFilledRect(box, color);*/

    x += well_padding_x;
    drawWellBackground(gcx, x, y);
    tex_ok->drawAt(
        x + (well_width - static_cast<int>(tex_ok->width())) / 2,
        y + (well_height - static_cast<int>(tex_ok->height())) / 2);
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
