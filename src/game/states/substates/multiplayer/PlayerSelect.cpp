#include "PlayerSelect.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "system/Color.h"
#include "system/GraphicsContext.h"

#include <assert.h>


namespace SubStates {
namespace Multiplayer {
namespace States {

static const int placeholder_width = 10 * Mino::texture_size_px;
static const int placeholder_height = 20 * Mino::texture_size_px;
static const int placeholder_padding = Mino::texture_size_px;

PlayerSelect::PlayerSelect()
{
    devices.push_back(-1);
}

void PlayerSelect::update(MultiplayerState&, const std::vector<Event>&, AppContext&)
{

}

void PlayerSelect::draw(MultiplayerState&, GraphicsContext& gcx) const
{
    assert(devices.size() > 0);

    static const int placeholder_full_width = placeholder_width + 2 * placeholder_padding;
    int well_x = (gcx.screenWidth() - placeholder_full_width * devices.size()) / 2;
    const int well_y = (gcx.screenHeight() - placeholder_height) / 2;
    for (const auto& device_id : devices) {
        drawWellPlaceholder(gcx, well_x, well_y);
        well_x += placeholder_full_width;
    }
}

void PlayerSelect::drawWellPlaceholder(GraphicsContext& gcx, int x, int y) const
{
    static const auto color = 0x1A3A8A_rgb;

    const Rectangle box = {x + placeholder_padding, y, placeholder_width, placeholder_height};
    gcx.drawFilledRect(box, color);
}

} // namespace States
} // namespace Multiplayer
} // namespace SubStates
