#include "OptionsItem.h"

#include "game/AppContext.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Paths.h"
#include "system/Texture.h"

namespace Layout {
namespace Options {

// darker accent to make white text more visible
RGBAColor OptionsItem::text_color = 0xEEEEEEFF_rgba;
RGBAColor OptionsItem::marker_color = 0xCE8000FF_rgba;

OptionsItem::OptionsItem(AppContext& app, std::string&& label, std::string&& description)
    : Layout::MenuItem(std::forward<std::string>(label))
    , margin_bottom(6)
{
    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 24);
    tex_label = font->renderText(btn_label_text, text_color);

    if (!description.empty()) {
        font = app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 20);
        tex_description = font->renderText(description, text_color);
    }
}

OptionsItem::~OptionsItem() = default;

} // namespace Options
} // namespace Layout
