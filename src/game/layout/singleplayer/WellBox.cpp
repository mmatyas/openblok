#include "WellBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "system/Font.h"
#include "system/Localize.h"


namespace Layout {

WellBox::WellBox(AppContext& app)
{
    m_well.setRotationFn(app.scripts().loadRotationFn("data/rotations/srs.lua"));

    bounding_box.w = 10 * Mino::texture_size_px + border_width * 2;
    bounding_box.h = 20 * Mino::texture_size_px + border_width * 2;

    font_big = app.gcx().loadFont("data/fonts/PTC75F.ttf", 60);
    tex_pause = font_big->renderText(tr("PAUSE"), 0xEEEEEE_rgb);

    setPosition(0, 0);
}

void WellBox::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;

    border_left = { x, y, border_width, height() };
    border_right = { x + width() - border_width, y, border_width, height() };
    border_top = { x + border_width, y, width() - border_width * 2, border_width };
    border_bottom = { x + border_width, y + height() - border_width,
                      width() - border_width * 2, border_width };
}

void WellBox::update(const std::vector<InputEvent>& events, AppContext& app)
{
    m_well.update(events, app);
}

void WellBox::draw(GraphicsContext& gcx, bool paused) const
{
    m_well.drawBackground(gcx, x() + border_width, y() + border_width);
    if (paused) {
        tex_pause->drawAt(x() + (width() - tex_pause->width()) / 2,
                          y() + (height() - tex_pause->height()) / 2);
    }
    else
        m_well.drawContent(gcx, x() + border_width, y() + border_width);

    static const auto boardborder_color = 0x1A3A8A_rgb;
    gcx.drawFilledRect(border_left, boardborder_color);
    gcx.drawFilledRect(border_right, boardborder_color);
    gcx.drawFilledRect(border_top, boardborder_color);
    gcx.drawFilledRect(border_bottom, boardborder_color);
}

} // namespace Layout
