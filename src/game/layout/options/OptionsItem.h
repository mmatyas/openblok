#pragma once

#include "game/layout/MenuItem.h"

#include <memory>

class AppContext;
class Texture;


namespace Layout {
namespace Options {

class OptionsItem : public Layout::MenuItem {
public:
    OptionsItem(AppContext&, std::string&& label);
    virtual ~OptionsItem();

    virtual void setWidth(int w) { bounding_box.w = w; }

    int marginBottom() const { return margin_bottom; }
    void setMarginBottom(int px) { margin_bottom = px; }

protected:
    std::unique_ptr<Texture> tex_label;

private:
    int margin_bottom;
};

} // namespace Options
} // namespace Layout
