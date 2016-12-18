#include "BottomBarBox.h"

#include "game/AppContext.h"
#include "game/components/Mino.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Paths.h"


namespace Layout {
BottomBarBox::BottomBarBox(AppContext& app)
{
    font_content = app.gcx().loadFont(Paths::data() + "fonts/PTN77F.ttf", 30);
    font_content_highlight = app.gcx().loadFont(Paths::data() + "fonts/PTN77F.ttf", 32);

    tex_level_counter = font_content->renderText(tr("LEVEL 0"), 0xEEEEEE_rgb);
    tex_score_counter = font_content->renderText("0", 0xEEEEEE_rgb);

    bounding_box.w = 10 + 10 * Mino::texture_size_px;
    bounding_box.h = tex_level_counter->height() + 2 * 5;

    setPosition(0, 0);
}

void BottomBarBox::setPosition(int x, int y)
{
    Box::setPosition(x, y);

    rect_level = { x, y, 5 * Mino::texture_size_px, height() };
    rect_score = {
        x + width() - 5 * Mino::texture_size_px, y,
        5 * Mino::texture_size_px, height()
    };
}

void BottomBarBox::updateLevelCounter(unsigned num)
{
    tex_level_counter = font_content->renderText(tr("LEVEL ") + std::to_string(num), 0xEEEEEE_rgb);
}

void BottomBarBox::updateScore(unsigned num)
{
    tex_score_counter = font_content->renderText(std::to_string(num), 0xEEEEEE_rgb);
}

void BottomBarBox::draw(GraphicsContext& gcx) const
{
    // level
    gcx.drawFilledRect(rect_level, box_color);
    tex_level_counter->drawAt(rect_level.x + 10, rect_level.y + 5);

    // score
    gcx.drawFilledRect(rect_score, box_color);
    tex_score_counter->drawAt(rect_score.x + rect_score.w - tex_score_counter->width() - 10,
                              rect_score.y + 5);
}
} // namespace Layout
