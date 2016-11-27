#include "OptionsItem.h"

#include "game/AppContext.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Paths.h"
#include "system/Texture.h"

namespace Layout {
namespace Options {

OptionsItem::OptionsItem(AppContext& app, std::string&& label, std::string&& description)
    : Layout::MenuItem(std::forward<std::string>(label))
    , margin_bottom(6)
{
    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 24);
    tex_label = font->renderText(btn_label_text, 0xEEEEEE_rgb);

    font = app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 20);
    tex_description = font->renderText(description, 0xEEEEEE_rgb);
}

OptionsItem::~OptionsItem() = default;

} // namespace Options
} // namespace Layout
