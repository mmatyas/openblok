#include "MainMenuBtn.h"

#include "game/AppContext.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Paths.h"
#include "system/Texture.h"


namespace Layout {

RGBAColor MainMenuButton::bg_active_color;

MainMenuButton::MainMenuButton(AppContext& app, std::string&& text, std::function<void()>&& on_press)
    : Button(std::forward<std::string>(text), std::forward<std::function<void()>>(on_press))
{
    MainMenuButton::bg_active_color = app.theme().colors.mainmenu_panel;

    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTC75F.ttf", 35);
    tex_label_on = font->renderText(btn_label_text, app.theme().colors.mainmenu_highlight);
    tex_label_off = font->renderText(btn_label_text, app.theme().colors.mainmenu_normal);

    bounding_box.w = 450;
    bounding_box.h = tex_label_on->height() + 10;
}

void MainMenuButton::setAlpha(uint8_t alpha)
{
    tex_label_on->setAlpha(alpha);
    tex_label_off->setAlpha(alpha);
}

void MainMenuButton::draw(GraphicsContext& gcx) const
{
    if (is_active) {
        gcx.drawFilledRect(bounding_box, bg_active_color);
        tex_label_on->drawAt(x() + 20, y() + 5);
    }
    else
        tex_label_off->drawAt(x() + 20, y() + 5);
}

} // namespace Layout
