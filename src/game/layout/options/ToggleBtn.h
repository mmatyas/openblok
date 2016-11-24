#pragma once

#include "game/layout/options/OptionsItem.h"

#include <array>
#include <memory>

class AppContext;
class Texture;


namespace Layout {
namespace Options {

class ToggleButton : public OptionsItem {
public:
    ToggleButton(AppContext&, std::string&& label, std::function<void()>&& on_press);

    void setPosition(int x, int y) override;
    void draw(GraphicsContext&) const override;

private:
    std::array<std::unique_ptr<Texture>, 2> tex_onoff;
};

} // namespace Options
} // namespace Layout
