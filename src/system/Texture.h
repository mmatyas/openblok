#pragma once

#include "Rectangle.h"

#include <stdint.h>


class Texture {
public:
    virtual ~Texture() {}

    /// Draw the texture at a location
    virtual void drawAt(int x, int y) = 0;
    /// Fill a rectangle with the texture
    virtual void drawScaled(const Rectangle&) = 0;
    /// Copy part of the texture and fill a rectangle with it
    virtual void drawPartialScaled(const Rectangle& from, const Rectangle& to) = 0;

    /// Set the current alpha value (visibility) of the texture
    virtual void setAlpha(uint8_t) = 0;
    /// Get the current alpha value (visibility) of the texture
    virtual uint8_t alpha() const = 0;

    /// Get the width of the texture
    virtual unsigned width() const = 0;
    /// Get the height of the texture
    virtual unsigned height() const = 0;
};
