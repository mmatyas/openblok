#include "GarbageGauge.h"

#include "game/AppContext.h"
#include "system/GraphicsContext.h"
#include "system/Texture.h"

#include <assert.h>


namespace Layout {

RGBAColor GarbageGauge::border_color;

GarbageGauge::GarbageGauge(AppContext& app, int height)
    : line_count(0)
    , cell_height((height - 2 * BORDER_WIDTH) / MAX_LINES)
    , tex_cell(app.gcx().loadTexture(app.theme().get_texture("garbage.png")))
{
    GarbageGauge::border_color = app.theme().colors.panel;

    assert(cell_height > 0);

    bounding_box.w = GAUGE_WIDTH + BORDER_WIDTH;
    bounding_box.h = height;

    setPosition(0, 0);
}

void GarbageGauge::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;

    border_right = { x + GAUGE_WIDTH, y, BORDER_WIDTH, height() };
    border_top = { x, y, width() - BORDER_WIDTH, BORDER_WIDTH };
    border_bottom = { x, y + height() - BORDER_WIDTH, width() - BORDER_WIDTH, BORDER_WIDTH };
}

void GarbageGauge::setLineCount(unsigned short num)
{
    // NOTE: I couldn't use std::max here for some reason...
    line_count = (num < MAX_LINES) ? num : MAX_LINES;
}

void GarbageGauge::drawPassive(GraphicsContext& gcx) const
{
    gcx.drawFilledRect(border_top, border_color);
    gcx.drawFilledRect(border_right, border_color);
    gcx.drawFilledRect(border_bottom, border_color);

    ::Rectangle cell_rect = {
        x(), y() + height() - BORDER_WIDTH + 2, GAUGE_WIDTH, cell_height - 4 };
    for (unsigned short i = 0; i < line_count; i++) {
        cell_rect.y -= cell_height;
        tex_cell->drawScaled(cell_rect);
    }
}

void GarbageGauge::drawActive(GraphicsContext&) const
{
}

} // namespace Layout
