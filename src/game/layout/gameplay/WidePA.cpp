#include "WidePA.h"

namespace Layout {

WidePA::WidePA(AppContext& app)
    : PlayerArea(app)
    , ui_left(app, ui_well.height())
    , ui_right(app, ui_well.height())
{
    bounding_box.w = ui_left.width() + ui_well.width() + ui_right.width() + 2 * inner_padding;
    bounding_box.h = ui_well.height();

    setPosition(0, 0);
}

void WidePA::update()
{
    ui_left.update();
    ui_right.update();
}

void WidePA::setPosition(int pos_x, int pos_y)
{
    Box::setPosition(pos_x, pos_y);

    ui_left.setPosition(x(), y());
    ui_well.setPosition(x() + ui_left.width() + inner_padding, y());
    ui_right.setPosition(ui_well.x() + ui_well.width() + inner_padding, y());

    PlayerArea::calcWellBox();
}

void WidePA::drawActive(GraphicsContext& gcx) const
{
    ui_well.drawContent(gcx);
    ui_left.drawActive(gcx);
    ui_right.drawActive(gcx);
}

void WidePA::drawPassive(GraphicsContext& gcx) const
{
    ui_well.drawBase(gcx);
    ui_left.drawPassive(gcx);
    ui_right.drawPassive(gcx);
}

void WidePA::updateGametime(Duration dt)
{
    ui_right.updateGametime(dt);
}

void WidePA::updateGoalCounter(unsigned num)
{
    ui_left.updateGoalCounter(num);
}

void WidePA::updateLevelCounter(unsigned num)
{
    ui_left.updateLevelCounter(num);
}

void WidePA::updateScore(unsigned num)
{
    ui_right.updateScore(num);
}

} // namespace Layout
