#include "RightSidebarBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "game/util/DurationToString.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"


namespace Layout {

RightSidebarBox::RightSidebarBox(AppContext& app, int height)
    : next_queue(5)
    , gametime_text("00:00")
{
    bounding_box.w = 5 * Mino::texture_size_px;
    bounding_box.h = height;

    // TODO: cache
    font_label = app.gcx().loadFont(Paths::data() + "fonts/PTN57F.ttf", 28);
    font_content = app.gcx().loadFont(Paths::data() + "fonts/PTN77F.ttf", 30);

    const auto color = 0xEEEEEE_rgb;
    tex_next = font_label->renderText(tr("NEXT"), color);
    tex_score = font_label->renderText(tr("SCORE"), color);
    tex_score_counter = font_content->renderText("0", color);
    tex_time_counter = font_content->renderText(gametime_text, color);

    setPosition(0, 0);
}

void RightSidebarBox::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;

    rect_score = {
        x, y + height() - text_height - text_padding * 2,
        width(), text_height + text_padding * 2 };
    rect_time = rect_score;
    rect_time.y = rect_score.y - text_padding * 2 - text_height - item_padding - rect_score.h;
}

void RightSidebarBox::update()
{}

void RightSidebarBox::updateScore(unsigned num)
{
    tex_score_counter = font_content->renderText(std::to_string(num), 0xEEEEEE_rgb);
}

void RightSidebarBox::updateGametime(Duration gametime)
{
    const auto newstr = Timing::toString(gametime);
    if (newstr != gametime_text) {
        gametime_text = newstr;
        tex_time_counter = font_content->renderText(gametime_text, 0xEEEEEE_rgb);
    }
}

void RightSidebarBox::drawPassive(GraphicsContext& gcx) const
{
    // next queue
    tex_next->drawAt(x() + width() - tex_next->width(), y());
    next_queue.draw(gcx, x(), y() + text_height + text_padding);

    // score
    gcx.drawFilledRect(rect_score, box_color);
    tex_score_counter->drawAt(rect_score.x + (rect_score.w - tex_score_counter->width()) / 2,
                              rect_score.y + 5);
    tex_score->drawAt(rect_score.x + width() - tex_score->width(),
                      rect_score.y - text_padding - text_height);

    // time
    gcx.drawFilledRect(rect_time, box_color);
    tex_time_counter->drawAt(rect_time.x + (rect_time.w - tex_time_counter->width()) / 2,
                             rect_time.y + 5);
}

void RightSidebarBox::drawActive(GraphicsContext&) const
{
}

} // namespace Layout
