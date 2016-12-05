#pragma once

#include "game/layout/options/OptionsItem.h"

#include <array>
#include <functional>


namespace Layout {
namespace Options {

class ToggleButton : public OptionsItem {
public:
    ToggleButton(AppContext&, bool initial_state,
                 std::string&& label, std::string&& description = "",
                 std::function<void(bool)>&& on_toggle = [](bool){});

    void onPress(AppContext&, InputType) override;
    void draw(GraphicsContext&) const override;

    void setWidth(int) override;

private:
    std::array<std::unique_ptr<Texture>, 2> tex_onoff;

    int leftswitch_centerx, rightswitch_centerx;
    int padding_ver;
    static constexpr int padding_left = 20;
    static constexpr int padding_right = 10;
    static constexpr int switch_half_width = 40;

    bool switch_state;
    void onLeftPress();
    void onRightPress();
    std::function<void(bool)> callback;
};

} // namespace Options
} // namespace Layout
