#pragma once

#include "game/layout/MenuItem.h"

#include <memory>

class AppContext;
class Texture;


namespace Layout {
namespace Options {

class CategoryButton : public Layout::MenuItem {
public:
    CategoryButton(AppContext&, std::string&& label, std::function<void()>&& on_press = [](){});

    void draw(GraphicsContext&) const override;

private:
    std::unique_ptr<Texture> tex_label;
};

} // namespace Options
} // namespace Layout
