#include "TopBarBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"


namespace Layout {

TopBarBox::TopBarBox(AppContext& app, int width)
    : next_queue(1)
{
    bounding_box.w = width;
    bounding_box.h = 4 * Mino::texture_size_px;

    auto font_label = app.gcx().loadFont(Paths::data() + "fonts/PTN57F.ttf", 28);
    tex_hold = font_label->renderText(tr("HOLD"), 0xEEEEEE_rgb);
    tex_next = font_label->renderText(tr("NEXT"), 0xEEEEEE_rgb);

    setPosition(0, 0);
}

void TopBarBox::update()
{
    hold_queue.update();
}

void TopBarBox::drawPassive(GraphicsContext& gcx) const
{
    // hold queue
    hold_queue.draw(gcx, x(), y());
    tex_hold->drawAt(x() + 5, y());

    // next queue
    next_queue.draw(gcx, x() + width() - 5 * Mino::texture_size_px, y());
    tex_next->drawAt(x() + width() - tex_next->width() - 5, y());
}

void TopBarBox::drawActive(GraphicsContext&) const
{
}

} // namespace Layout
