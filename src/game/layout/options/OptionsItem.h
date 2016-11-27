#pragma once

#include "game/layout/MenuItem.h"

#include <memory>

class AppContext;
class Texture;


namespace Layout {
namespace Options {

class OptionsItem : public Layout::MenuItem {
public:
    OptionsItem(AppContext&, std::string&& label, std::string&& description);
    virtual ~OptionsItem();

    virtual void setWidth(int w) { bounding_box.w = w; }

    int marginBottom() const { return margin_bottom; }
    void setMarginBottom(int px) { margin_bottom = px; }

    virtual void onLeftPress() = 0;
    virtual void onRightPress() = 0;

    const std::unique_ptr<Texture>& descriptionTex() { return tex_description; }

protected:
    std::unique_ptr<Texture> tex_label;
    std::unique_ptr<Texture> tex_description;

private:
    int margin_bottom;
};

} // namespace Options
} // namespace Layout
