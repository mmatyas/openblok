#pragma once

#include "system/Rectangle.h"


namespace Layout {
class Box {
public:
    Box() : bounding_box() {}
    virtual ~Box() = default;

    virtual void setPosition(int x, int y) {
        bounding_box.x = x;
        bounding_box.y = y;
    }

    int x() const { return bounding_box.x; }
    int y() const { return bounding_box.y; }
    int width() const { return bounding_box.w; }
    int height() const { return bounding_box.h; }

protected:
    ::Rectangle bounding_box;
};
}
