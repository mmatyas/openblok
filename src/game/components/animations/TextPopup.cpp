#include "TextPopup.h"

#include "game/AppContext.h"
#include "system/Font.h"
#include "system/Texture.h"


RGBColor TextPopup::text_color = 0xEEEEEE_rgb;

TextPopup::TextPopup(const std::string& text, std::shared_ptr<Font>& font)
    : text(text)
    , pos_x(0)
    , pos_y(0)
    , pos_y_delta(std::chrono::seconds(2), [](double t){
            return t * 100;
        })
    , alpha(pos_y_delta.length(), [](double t){
            return (1.0 - t) * 0xFF;
        })
{
    tex = font->renderText(text, text_color);
    tex->setAlpha(0x0);
}

void TextPopup::setInitialPosition(int x, int y)
{
    pos_x = x;
    pos_y = y;
    tex->setAlpha(0xFF);
}

unsigned TextPopup::width() const
{
    return tex->width();
}

void TextPopup::update()
{
    pos_y_delta.update(Timing::frame_duration);
    alpha.update(Timing::frame_duration);
    tex->setAlpha(alpha.value());
}

void TextPopup::draw() const
{
    tex->drawAt(pos_x, pos_y - pos_y_delta.value());
}
