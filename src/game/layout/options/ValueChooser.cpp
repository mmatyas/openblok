#include "ValueChooser.h"

#include "game/AppContext.h"
#include "system/GraphicsContext.h"
#include "system/Font.h"
#include "system/Paths.h"
#include "system/Texture.h"

#include <assert.h>


namespace Layout {
namespace Options {

ValueChooser::ValueChooser(AppContext& app,
                           std::vector<std::string>&& possible_values, size_t start_index,
                           std::string&& text, std::string&& description,
                           std::function<void(const std::string)>&& on_change)
    : OptionsItem(app, std::forward<std::string>(text), std::forward<std::string>(description))
    , values(possible_values)
    , current_idx(start_index)
    , callback(on_change)
{
    bounding_box.h = 40;
    setWidth(750);

    auto font = app.gcx().loadFont(Paths::data() + "fonts/PTS55F.ttf", 24);

    assert(values.size() > 1);
    assert(current_idx < values.size());
    for (const auto& val : values)
        tex_inactives.emplace_back(font->renderText(val, 0xEEEEEE_rgb));

    // put a '<' or '>' around the text when the item is selected
    tex_actives.emplace_back(font->renderText(values.front() + " \u203A", 0xEEEEEE_rgb));
    for (unsigned i = 1; i < values.size() - 1; i++)
        tex_actives.emplace_back(font->renderText("\u2039 " + values.at(i) + " \u203A", 0xEEEEEE_rgb));
    tex_actives.emplace_back(font->renderText("\u2039 " + values.back(), 0xEEEEEE_rgb));

    padding_ver = (height() - tex_label->height()) / 2;
}

void ValueChooser::setWidth(int w)
{
    OptionsItem::setWidth(w);
    right_x = x() + width() - padding_hor;
}

void ValueChooser::onLeftPress()
{
    if (current_idx > 0) {
        current_idx--;
        callback(values.at(current_idx));
    }
}

void ValueChooser::onRightPress()
{
    if (current_idx + 1 < values.size()) {
        current_idx++;
        callback(values.at(current_idx));
    }
}

void ValueChooser::draw(GraphicsContext& gcx) const
{
    assert(current_idx < values.size());
    if (is_active) {
        gcx.drawFilledRect(bounding_box, 0x0060BF_rgb);
        gcx.drawFilledRect({x(), y(), 6, height()}, 0xCE8000_rgb);
        tex_label->drawAt(x() + padding_hor, y() + padding_ver);

        tex_actives.at(current_idx)->drawAt(right_x - tex_actives.at(current_idx)->width(), y() + padding_ver);
    }
    else {
        gcx.drawFilledRect(bounding_box, 0x002687_rgb);
        tex_label->drawAt(x() + padding_hor, y() + padding_ver);

        tex_inactives.at(current_idx)->drawAt(right_x - tex_inactives.at(current_idx)->width(), y() + padding_ver);
    }
}

} // namespace Options
} // namespace Layout

