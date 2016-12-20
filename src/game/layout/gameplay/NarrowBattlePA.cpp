#include "NarrowBattlePA.h"

namespace Layout {

NarrowBattlePA::NarrowBattlePA(AppContext& app)
    : PlayerArea(app)
    , ui_gauge(app, ui_well.height())
    , ui_top(app, ui_well.width() + ui_gauge.width())
    , ui_bottom(app, ui_well.width() + ui_gauge.width())
{
    bounding_box.w = ui_well.width() + ui_gauge.width();
    bounding_box.h = ui_top.height() + ui_well.height() + ui_bottom.height() + 2 * inner_padding;

    setPosition(0, 0);
}

void NarrowBattlePA::update()
{
    ui_top.update();
}

void NarrowBattlePA::setPosition(int pos_x, int pos_y)
{
    Box::setPosition(pos_x, pos_y);

    ui_top.setPosition(x(), y());
    ui_well.setPosition(x(), y() + ui_top.height() + inner_padding);
    ui_gauge.setPosition(x() + ui_well.width(), ui_well.y());
    ui_bottom.setPosition(x(), ui_well.y() + ui_well.height() + inner_padding);

    PlayerArea::calcWellBox();
}

void NarrowBattlePA::drawActive(GraphicsContext& gcx) const
{
    ui_well.drawContent(gcx);
    ui_gauge.drawActive(gcx);
    ui_top.drawActive(gcx);
    ui_bottom.drawActive(gcx);
}

void NarrowBattlePA::drawPassive(GraphicsContext& gcx) const
{
    ui_well.drawBase(gcx);
    ui_gauge.drawPassive(gcx);
    ui_top.drawPassive(gcx);
    ui_bottom.drawPassive(gcx);
}

void NarrowBattlePA::updateLevelCounter(unsigned num)
{
    ui_bottom.updateLevelCounter(num);
}

void NarrowBattlePA::updateScore(unsigned num)
{
    ui_bottom.updateScore(num);
}

void NarrowBattlePA::updateGarbageGauge(unsigned lines)
{
    ui_gauge.setLineCount(lines);
}

} // namespace Layout

