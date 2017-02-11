#include "CategoryBtn.h"

#include "game/AppContext.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Paths.h"
#include "system/Texture.h"


namespace Layout {
namespace Options {

RGBAColor CategoryButton::text_color = 0xEEEEEEFF_rgba;
RGBAColor CategoryButton::marker_color = 0xCE8000FF_rgba;
RGBAColor CategoryButton::bg_active_color = 0x0060BFFF_rgba;
RGBAColor CategoryButton::bg_passive_color = 0x002687FF_rgba;

CategoryButton::CategoryButton(AppContext& app, std::string&& text)
    : Layout::MenuItem(std::forward<std::string>(text))
{
    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 30);
    tex_label = font->renderText(btn_label_text, text_color);

    bounding_box.w = 270;
    bounding_box.h = 90;
}

void CategoryButton::draw(GraphicsContext& gcx) const
{
    const int category_btn_padding = (height() - tex_label->height()) / 2;
    if (is_active) {
        gcx.drawFilledRect(bounding_box, bg_active_color);
        gcx.drawFilledRect({x(), y(), 10, height()}, marker_color);
        tex_label->drawAt(x() + 40, y() + category_btn_padding);
    }
    else {
        gcx.drawFilledRect(bounding_box, bg_passive_color);
        tex_label->drawAt(x() + 40, y() + category_btn_padding);
    }
}

} // namespace Options
} // namespace Layout
