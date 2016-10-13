#include "RightSidebarBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "system/Localize.h"


namespace Layout {

RightSidebarBox::RightSidebarBox(AppContext& app, uint16_t height)
    : next_queue(4)
    , gametime(Duration::zero())
    , gametime_text("00:00")
{
    bounding_box.w = 5 * Mino::texture_size_px;
    bounding_box.h = height;

    // TODO: cache
    font_label = app.gcx().loadFont("data/fonts/PTN57F.ttf", 28);
    font_content = app.gcx().loadFont("data/fonts/PTN77F.ttf", 30);

    tex_next = app.gcx().renderText(tr("NEXT"), font_label, 0xEEEEEE_rgb);
    tex_score = app.gcx().renderText(tr("SCORE"), font_label, 0xEEEEEE_rgb);
    tex_score_counter = app.gcx().renderText("0", font_content, 0xEEEEEE_rgb);
    tex_time_counter = app.gcx().renderText(gametime_text, font_content, 0xEEEEEE_rgb);
}

void RightSidebarBox::setPosition(uint16_t x, uint16_t y)
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

void RightSidebarBox::updateScore(GraphicsContext& gcx, unsigned num)
{
    gcx.renderText(tex_score_counter, std::to_string(num),
                   font_content, 0xEEEEEE_rgb);
}

void RightSidebarBox::updateGametime(GraphicsContext& gcx)
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
        gcx.renderText(tex_time_counter, gametime_text, font_content, 0xEEEEEE_rgb);
    }
}

void RightSidebarBox::draw(GraphicsContext& gcx) const
{
    // next queue
    gcx.drawTexture(tex_next, x() + width() - gcx.textureWidth(tex_next), y());
    next_queue.draw(gcx, x(), y() + text_height + text_padding);

    // score
    gcx.drawFilledRect(rect_score, box_color);
    gcx.drawTexture(tex_score_counter,
                    rect_score.x + (rect_score.w - gcx.textureWidth(tex_score_counter)) / 2,
                    rect_score.y + 3);
    gcx.drawTexture(tex_score,
                    rect_score.x + width() - gcx.textureWidth(tex_score),
                    rect_score.y - text_padding - text_height);

    // time
    gcx.drawFilledRect(rect_time, box_color);
    gcx.drawTexture(tex_time_counter,
                    rect_time.x + (rect_time.w - gcx.textureWidth(tex_time_counter)) / 2,
                    rect_time.y + 3);
}

} // namespace Layout
