#include "RightSidebarBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "system/Font.h"
#include "system/Localize.h"


namespace Layout {

RightSidebarBox::RightSidebarBox(AppContext& app, int height)
    : next_queue(5)
    , gametime(Duration::zero())
    , gametime_text("00:00")
{
    bounding_box.w = 5 * Mino::texture_size_px;
    bounding_box.h = height;

    // TODO: cache
    font_label = app.gcx().loadFont(DATADIR + "fonts/PTN57F.ttf", 28);
    font_content = app.gcx().loadFont(DATADIR + "fonts/PTN77F.ttf", 30);

    tex_next = font_label->renderText(tr("NEXT"), 0xEEEEEE_rgb);
    tex_score = font_label->renderText(tr("SCORE"), 0xEEEEEE_rgb);
    tex_score_counter = font_content->renderText("0", 0xEEEEEE_rgb);
    tex_time_counter = font_content->renderText(gametime_text, 0xEEEEEE_rgb);
}

void RightSidebarBox::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;

    rect_score = { x, y + height() - text_height - text_padding * 2,
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

void RightSidebarBox::updateGametime()
{
    gametime += Timing::frame_duration;

    unsigned minutes = std::chrono::duration_cast<std::chrono::minutes>(gametime).count();
    unsigned seconds = std::chrono::duration_cast<std::chrono::seconds>(gametime).count() % 60;
    std::string newstr;
    if (minutes < 10)
        newstr += "0";
    newstr += std::to_string(minutes);
    newstr += ":";
    if (seconds < 10)
        newstr += "0";
    newstr += std::to_string(seconds);

    // render text
    if (newstr != gametime_text) {
        gametime_text = newstr;
        tex_time_counter = font_content->renderText(gametime_text, 0xEEEEEE_rgb);
    }
}

void RightSidebarBox::draw(GraphicsContext& gcx) const
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

} // namespace Layout
