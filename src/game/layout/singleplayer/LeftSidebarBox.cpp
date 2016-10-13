#include "LeftSidebarBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "system/Localize.h"


namespace Layout {

LeftSidebarBox::LeftSidebarBox(AppContext& app, uint16_t height)
{
    bounding_box.w = 5 * Mino::texture_size_px;
    bounding_box.h = height;

    // TODO: cache
    font_label = app.gcx().loadFont("data/fonts/PTN57F.ttf", 28);
    font_content = app.gcx().loadFont("data/fonts/PTN77F.ttf", 30);

    tex_hold = app.gcx().renderText(tr("HOLD"), font_label, 0xEEEEEE_rgb);
    tex_goal = app.gcx().renderText(tr("GOAL"), font_label, 0xEEEEEE_rgb);
    tex_level = app.gcx().renderText(tr("LEVEL"), font_label, 0xEEEEEE_rgb);
    tex_goal_counter = app.gcx().renderText("0", font_content, 0xEEEEEE_rgb);
    tex_level_counter = app.gcx().renderText("0", font_content, 0xEEEEEE_rgb);

    setPosition(0, 0);
}

void LeftSidebarBox::setPosition(uint16_t x, uint16_t y)
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

void LeftSidebarBox::updateGoalCounter(GraphicsContext& gcx, unsigned num)
{
    gcx.renderText(tex_goal_counter, std::to_string(num),
                   font_content, 0xEEEEEE_rgb);
}

void LeftSidebarBox::updateLevelCounter(GraphicsContext& gcx, unsigned num)
{
    gcx.renderText(tex_level_counter, std::to_string(num),
                   font_content, 0xEEEEEE_rgb);
}

void LeftSidebarBox::draw(GraphicsContext& gcx) const
{
    // hold queue
    gcx.drawTexture(tex_hold, x(), y());
    hold_queue.draw(gcx, x(), y() + text_height + text_padding);

    // goal
    gcx.drawFilledRect(rect_goal, box_color);
    gcx.drawTexture(tex_goal_counter,
                      rect_goal.x + (rect_goal.w - gcx.textureWidth(tex_goal_counter)) / 2,
                      rect_goal.y + 3);
    gcx.drawTexture(tex_goal, rect_goal.x, rect_goal.y - text_padding - text_height);

    // level
    gcx.drawFilledRect(rect_level, box_color);
    gcx.drawTexture(tex_level_counter,
                      rect_level.x + (rect_level.w - gcx.textureWidth(tex_level_counter)) / 2,
                      rect_level.y + 3);
    gcx.drawTexture(tex_level, rect_level.x, rect_level.y - text_padding - text_height);
}

} // namespace Layout
