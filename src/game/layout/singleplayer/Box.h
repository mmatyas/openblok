#pragma once

#include "system/GraphicsContext.h"


namespace Layout {
class Box {
public:
    Box() : bounding_box() {}
    virtual ~Box() = default;

    virtual void setPosition(uint16_t x, uint16_t y) = 0;

    uint16_t x() const { return bounding_box.x; }
    uint16_t y() const { return bounding_box.y; }
    uint16_t width() const { return bounding_box.w; }
    uint16_t height() const { return bounding_box.h; }

protected:
    ::Rectangle bounding_box;
};
}
