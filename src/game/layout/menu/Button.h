#pragma once

#include "game/layout/MenuItem.h"

#include <functional>


class GraphicsContext;


namespace Layout {
class Button : public MenuItem {
public:
    Button(std::string&& label, std::function<void()>&& on_press = [](){})
        : MenuItem(std::forward<std::string>(label))
        , btn_on_press(on_press)
    {}

    virtual void onPress() { btn_on_press(); }

protected:
    std::function<void()> btn_on_press;
};
} // namespace Layout
