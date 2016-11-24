#pragma once

#include "Box.h"

#include <string>

class GraphicsContext;


namespace Layout {
class MenuItem : public Box {
public:
    MenuItem(std::string&& label)
        : btn_label_text(label)
        , is_active(false)
    {}

    virtual void onHoverEnter() { is_active = true; }
    virtual void onHoverLeave() { is_active = false; }

    virtual void update() {}
    virtual void draw(GraphicsContext&) const = 0;

protected:
    std::string btn_label_text;
    bool is_active;
};
} // namespace Layout
