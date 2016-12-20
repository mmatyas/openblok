#include "WellContainer.h"

#include "game/AppContext.h"
#include "game/components/rotations/RotationFactory.h"
#include "system/GraphicsContext.h"


namespace Layout {

WellContainer::WellContainer(AppContext& app)
    : m_well(app.wellconfig())
{
    bounding_box.w = wellWidth() + border_width * 2;
    bounding_box.h = wellHeight() + border_width * 2;

    setPosition(0, 0);
}

void WellContainer::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;

    border_left = { x, y, border_width, height() };
    border_right = { x + width() - border_width, y, border_width, height() };
    border_top = { x + border_width, y, width() - border_width * 2, border_width };
    border_bottom = { x + border_width, y + height() - border_width,
                      width() - border_width * 2, border_width };
}

void WellContainer::drawBase(GraphicsContext& gcx) const
{
    m_well.drawBackground(gcx, x() + border_width, y() + border_width);

    static const auto boardborder_color = 0x1A3A8A_rgb;
    gcx.drawFilledRect(border_left, boardborder_color);
    gcx.drawFilledRect(border_right, boardborder_color);
    gcx.drawFilledRect(border_top, boardborder_color);
    gcx.drawFilledRect(border_bottom, boardborder_color);
}

void WellContainer::drawContent(GraphicsContext& gcx) const
{
    m_well.drawContent(gcx, x() + border_width, y() + border_width);
}

} // namespace Layout

