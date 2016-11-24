#include "ToggleBtn.h"

#include "game/AppContext.h"
#include "system/GraphicsContext.h"
#include "system/Localize.h"
#include "system/Font.h"
#include "system/Paths.h"
#include "system/Texture.h"


namespace Layout {
namespace Options {

ToggleButton::ToggleButton(AppContext& app, bool initial_state, std::string&& text)
    : OptionsItem(app, std::forward<std::string>(text))
    , switch_state(initial_state)
{
    bounding_box.w = 750;
    bounding_box.h = 60;

    padding = (height() - tex_label->height()) / 2;

    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 28);
    tex_onoff.at(0) = font->renderText(tr("ON"), 0xEEEEEE_rgb);
    tex_onoff.at(1) = font->renderText(tr("OFF"), 0xEEEEEE_rgb);
}

void ToggleButton::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;
}

void ToggleButton::draw(GraphicsContext& gcx) const
{
    static const int padding_hor = 20;
    if (is_active) {
        gcx.drawFilledRect(bounding_box, 0x0060BF_rgb);
        gcx.drawFilledRect({x(), y(), 6, height()}, 0xCE8000_rgb);
        tex_label->drawAt(x() + padding_hor, y() + padding);
    }
    else {
        gcx.drawFilledRect(bounding_box, 0x002687_rgb);
        tex_label->drawAt(x() + padding_hor, y() + padding);
    }

    const int rightswitch_centerx = x() + width() - padding_hor - 50;
    const int leftswitch_centerx = rightswitch_centerx - 110;

    if (switch_state)
        gcx.drawFilledRect({leftswitch_centerx - 50, y() + 5, 100, 50}, 0xCE8000_rgb);
    else
        gcx.drawFilledRect({rightswitch_centerx - 50, y() + 5, 100, 50}, 0xCE8000_rgb);

    tex_onoff.at(0)->drawAt(leftswitch_centerx - tex_onoff.at(0)->width() / 2, y() + padding);
    tex_onoff.at(1)->drawAt(rightswitch_centerx - tex_onoff.at(1)->width() / 2, y() + padding);
}

} // namespace Options
} // namespace Layout
