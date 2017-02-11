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

private:
    std::unique_ptr<Texture> tex_label;

    static RGBAColor text_color;
    static RGBAColor marker_color;
    static RGBAColor bg_active_color;
    static RGBAColor bg_passive_color;
};

} // namespace Options
} // namespace Layout
