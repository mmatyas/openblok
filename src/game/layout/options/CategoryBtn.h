#pragma once

#include "game/layout/MenuItem.h"
#include "system/Color.h"

#include <memory>

class AppContext;
class Texture;


namespace Layout {
namespace Options {

class CategoryButton : public Layout::MenuItem {
public:
    CategoryButton(AppContext&, std::string&& label);

    void draw(GraphicsContext&) const override;

    static RGBColor marker_color;

private:
    std::unique_ptr<Texture> tex_label;
};

} // namespace Options
} // namespace Layout
