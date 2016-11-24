#include "CategoryBtn.h"

#include "game/AppContext.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Paths.h"
#include "system/Texture.h"


namespace Layout {
namespace Options {

CategoryButton::CategoryButton(AppContext& app, std::string&& text, std::function<void()>&& on_press)
    : Layout::MenuItem(std::forward<std::string>(text))
{
    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTS75F.ttf", 30);
    tex_label = font->renderText(btn_label_text, 0xEEEEEE_rgb);

    bounding_box.w = 270;
    bounding_box.h = 90;
}

void CategoryButton::draw(GraphicsContext& gcx) const
{
    const int category_btn_padding = (height() - tex_label->height()) / 2;
    if (is_active) {
        gcx.drawFilledRect(bounding_box, 0x0060BF_rgb);
        gcx.drawFilledRect({x(), y(), 10, height()}, 0xCE8000_rgb);
        tex_label->drawAt(x() + 40, y() + category_btn_padding);
    }
    else {
        gcx.drawFilledRect(bounding_box, 0x0030A8_rgb);
        tex_label->drawAt(x() + 40, y() + category_btn_padding);
    }
}

} // namespace Options
} // namespace Layout
