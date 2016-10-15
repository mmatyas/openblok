#include "LeftSidebarBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "system/Font.h"
#include "system/Localize.h"


namespace Layout {

LeftSidebarBox::LeftSidebarBox(AppContext& app, int height)
{
    bounding_box.w = 5 * Mino::texture_size_px;
    bounding_box.h = height;

    // TODO: cache
    font_label = app.gcx().loadFont("data/fonts/PTN57F.ttf", 28);
    font_content = app.gcx().loadFont("data/fonts/PTN77F.ttf", 30);

    tex_hold = font_label->renderText(tr("HOLD"), 0xEEEEEE_rgb);
    tex_goal = font_label->renderText(tr("GOAL"), 0xEEEEEE_rgb);
    tex_level = font_label->renderText(tr("LEVEL"), 0xEEEEEE_rgb);
    tex_goal_counter = font_content->renderText("0", 0xEEEEEE_rgb);
    tex_level_counter = font_content->renderText("0", 0xEEEEEE_rgb);

    setPosition(0, 0);
}

void LeftSidebarBox::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;

    rect_goal = { x, y + height() - text_height - text_padding * 2,
                  width(), text_height + text_padding * 2 };
    rect_level = rect_goal;
    rect_level.y = rect_goal.y - text_padding * 2 - text_height - item_padding - rect_goal.h;
}

void LeftSidebarBox::update()
{
    hold_queue.update();
}

void LeftSidebarBox::updateGoalCounter(unsigned num)
{
    tex_goal_counter = font_content->renderText(std::to_string(num), 0xEEEEEE_rgb);
}

void LeftSidebarBox::updateLevelCounter(unsigned num)
{
    tex_level_counter = font_content->renderText(std::to_string(num), 0xEEEEEE_rgb);
}

void LeftSidebarBox::draw(GraphicsContext& gcx) const
{
    // hold queue
    tex_hold->drawAt(x(), y());
    hold_queue.draw(gcx, x(), y() + text_height + text_padding);

    // goal
    gcx.drawFilledRect(rect_goal, box_color);
    tex_goal_counter->drawAt(rect_goal.x + (rect_goal.w - tex_goal_counter->width()) / 2,
                             rect_goal.y + 5);
    tex_goal->drawAt(rect_goal.x, rect_goal.y - text_padding - text_height);

    // level
    gcx.drawFilledRect(rect_level, box_color);
    tex_level_counter->drawAt(rect_level.x + (rect_level.w - tex_level_counter->width()) / 2,
                              rect_level.y + 5);
    tex_level->drawAt(rect_level.x, rect_level.y - text_padding - text_height);
}

} // namespace Layout
