#include "MainMenuBtn.h"

#include "game/AppContext.h"
#include "system/Font.h"
#include "system/GraphicsContext.h"
#include "system/Texture.h"


namespace Layout {

MainMenuButton::MainMenuButton(AppContext& app, std::string&& text, std::function<void()>&& on_press)
    : Button(std::forward<std::string>(text), std::forward<std::function<void()>>(on_press))
{
    auto font = app.gcx().loadFont("data/fonts/PTC75F.ttf", 35);
    tex_label_on = font->renderText(btn_label_text, 0xEEEEEE_rgb);
    tex_label_off = font->renderText(btn_label_text, 0x006080_rgb);

    bounding_box.w = 450;
    bounding_box.h = tex_label_on->height() + 10;
}

void MainMenuButton::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;
}

void MainMenuButton::update()
{

}

void MainMenuButton::draw(GraphicsContext& gcx) const
{
    if (is_active) {
        gcx.drawFilledRect(bounding_box, 0x2030FF_rgb);
        tex_label_on->drawAt(x() + 20, y() + 5);
    }
    else {
        tex_label_off->drawAt(x() + 20, y() + 5);
    }
    //gcx.drawFilledRect(bounding_box, 0x39CCCC_rgb);
}

} // namespace Layout
