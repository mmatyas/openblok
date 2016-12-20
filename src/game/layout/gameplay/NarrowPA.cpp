#include "NarrowPA.h"

namespace Layout {

NarrowPA::NarrowPA(AppContext& app)
    : PlayerArea(app)
    , ui_top(app, ui_well.width())
    , ui_bottom(app, ui_well.width())
{
    bounding_box.w = ui_well.width();
    bounding_box.h = ui_top.height() + ui_well.height() + ui_bottom.height() + 2 * inner_padding;

    setPosition(0, 0);
}

void NarrowPA::update()
{
    ui_top.update();
}

void NarrowPA::setPosition(int pos_x, int pos_y)
{
    Box::setPosition(pos_x, pos_y);

    ui_top.setPosition(x(), y());
    ui_well.setPosition(x(), y() + ui_top.height() + inner_padding);
    ui_bottom.setPosition(x(), ui_well.y() + ui_well.height() + inner_padding);

    PlayerArea::calcWellBox();
}

void NarrowPA::drawActive(GraphicsContext& gcx) const
{
    ui_well.drawContent(gcx);
    ui_top.drawActive(gcx);
    ui_bottom.drawActive(gcx);
}

void NarrowPA::drawPassive(GraphicsContext& gcx) const
{
    ui_well.drawBase(gcx);
    ui_top.drawPassive(gcx);
    ui_bottom.drawPassive(gcx);
}

void NarrowPA::updateLevelCounter(unsigned num)
{
    ui_bottom.updateLevelCounter(num);
}

void NarrowPA::updateScore(unsigned num)
{
    ui_bottom.updateScore(num);
}

} // namespace Layout
