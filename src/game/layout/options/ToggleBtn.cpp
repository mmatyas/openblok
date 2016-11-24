#include "ToggleBtn.h"

#include "system/GraphicsContext.h"
#include "system/Texture.h"


namespace Layout {
namespace Options {

ToggleButton::ToggleButton(AppContext& app, std::string&& text, std::function<void()>&& on_press)
    : OptionsItem(app, std::forward<std::string>(text), std::forward<std::function<void()>>(on_press))
{
    bounding_box.w = 750;
    bounding_box.h = 60;
}

void ToggleButton::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;
}

void ToggleButton::draw(GraphicsContext& gcx) const
{
    const int padding = (height() - tex_label->height()) / 2;
    if (is_active) {
        gcx.drawFilledRect(bounding_box, 0x0060BF_rgb);
        gcx.drawFilledRect({x(), y(), 6, height()}, 0xCE8000_rgb);
        tex_label->drawAt(x() + 20, y() + padding);
    }
    else {
        gcx.drawFilledRect(bounding_box, 0x002687_rgb);
        tex_label->drawAt(x() + 20, y() + padding);
    }
}

} // namespace Options
} // namespace Layout
