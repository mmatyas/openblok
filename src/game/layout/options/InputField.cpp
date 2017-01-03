#include "InputField.h"

#include "game/AppContext.h"
#include "system/GraphicsContext.h"
#include "system/Font.h"
#include "system/Paths.h"
#include "system/Texture.h"

#include <assert.h>


namespace Layout {
namespace Options {

InputField::InputField(AppContext& app, std::string&& label,
                       DeviceID device, uint16_t default_raw_key,
                       std::function<void(RawInputEvent)>&& on_change)
    : OptionsItem(app, std::forward<std::string>(label), "")
    , device_id(device)
    , waiting_for_input(false)
    , font(app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 24))
    , callback(on_change)
{
    bounding_box.h = 40;
    setWidth(750);
    padding_ver = (height() - tex_label->height()) / 2;

    changeButtonTex(app, default_raw_key);
}

void InputField::changeButtonTex(AppContext& app, uint16_t raw_key)
{
    tex_buttonname = font->renderText(app.window().buttonName(device_id, raw_key), 0xEEEEEE_rgb);
}

void InputField::setWidth(int w)
{
    OptionsItem::setWidth(w);
    right_x = x() + width() - padding_hor;
}

void InputField::onPress(AppContext&, InputType input)
{
    if (isLocked())
        return;

    switch (input) {
        case InputType::MENU_OK:
            waiting_for_input = true;
            break;
        default: break;
    }
}

void InputField::onRawPress(AppContext& app, RawInputEvent raw_input)
{
    if (!isLocked())
        return;

    if (raw_input.device_id != device_id)
        return;

    waiting_for_input = false;
    changeButtonTex(app, raw_input.button);

    callback(raw_input);
}

void InputField::draw(GraphicsContext& gcx) const
{
    if (is_active) {
        if (isLocked())
            gcx.drawFilledRect(bounding_box, 0xCE8000_rgb);
        else {
            gcx.drawFilledRect(bounding_box, 0x0060BF_rgb);
            gcx.drawFilledRect({x(), y(), 6, height()}, 0xCE8000_rgb);
        }
    }
    else
        gcx.drawFilledRect(bounding_box, 0x002687_rgb);

    tex_label->drawAt(x() + padding_hor, y() + padding_ver);
    tex_buttonname->drawAt(right_x - tex_buttonname->width(), y() + padding_ver);
}

} // namespace Options
} // namespace Layout
