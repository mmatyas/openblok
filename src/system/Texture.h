#pragma once

#include "Rectangle.h"

#include <stdint.h>


class Texture {
public:
    virtual ~Texture() {}

    virtual void drawAt(int x, int y) = 0;
    virtual void drawScaled(const Rectangle&) = 0;
    virtual void drawPartialScaled(const Rectangle& from, const Rectangle& to) = 0;

    virtual void setAlpha(uint8_t) = 0;
    virtual uint8_t alpha() const = 0;

    virtual unsigned width() const = 0;
    virtual unsigned height() const = 0;
};
