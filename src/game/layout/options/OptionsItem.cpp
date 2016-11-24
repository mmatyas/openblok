#include "OptionsItem.h"

#include "game/AppContext.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Paths.h"
#include "system/Texture.h"

namespace Layout {
namespace Options {

OptionsItem::OptionsItem(AppContext& app, std::string&& label, std::function<void()>&& on_press)
    : Layout::Button(std::forward<std::string>(label), std::forward<std::function<void()>>(on_press))
    , margin_bottom(0)
{
    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 30);
    tex_label = font->renderText(btn_label_text, 0xEEEEEE_rgb);
}

OptionsItem::~OptionsItem() = default;

void OptionsItem::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;
}

} // namespace Options
} // namespace Layout
