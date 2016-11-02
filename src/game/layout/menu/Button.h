#pragma once

#include "game/layout/Box.h"

#include <functional>
#include <string>


class GraphicsContext;


namespace Layout {
class Button : public Box {
public:
    Button(std::string&& label, std::function<void()>&& on_press = [](){})
        : btn_label_text(label)
        , btn_on_press(on_press)
        , is_active(false)
    {}

    virtual void onPress() { btn_on_press(); }

    virtual void onHoverEnter() { is_active = true; }
    virtual void onHoverLeave() { is_active = false; }

    virtual void update() = 0;
    virtual void draw(GraphicsContext&) const = 0;

protected:
    std::string btn_label_text;
    std::function<void()> btn_on_press;
    bool is_active;
};
} // namespace Layout
