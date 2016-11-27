#pragma once

#include "game/layout/options/OptionsItem.h"

#include <array>
#include <functional>
#include <memory>

class AppContext;
class Texture;


namespace Layout {
namespace Options {

class ToggleButton : public OptionsItem {
public:
    ToggleButton(AppContext&, bool initial_state,
                 std::string&& label, std::string&& description = " ",
                 std::function<void(bool)>&& on_toggle = [](bool){});

    void setPosition(int x, int y) override;
    void draw(GraphicsContext&) const override;

    void setWidth(int) override;
    void onLeftPress() override;
    void onRightPress() override;

private:
    std::array<std::unique_ptr<Texture>, 2> tex_onoff;

    int leftswitch_centerx, rightswitch_centerx;
    int padding_ver;
    static constexpr int padding_hor = 20;

    bool switch_state;
    std::function<void(bool)> callback;
};

} // namespace Options
} // namespace Layout
