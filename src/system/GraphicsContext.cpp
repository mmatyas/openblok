#include "GraphicsContext.h"

#include "sdl/SDLGraphicsContext.h"

#include <assert.h>


std::array<uint8_t, 3> operator "" _rgb(unsigned long long color) {
    assert(color <= 0xFFFFFF); // "RGB numeric value should be under 0xFFFFFF
    return {
        static_cast<uint8_t>((color & 0xFF0000) >> 16),
        static_cast<uint8_t>((color & 0xFF00) >> 8),
        static_cast<uint8_t>(color & 0xFF)
    };
}

std::array<uint8_t, 4> operator "" _rgba(unsigned long long color) {
    assert(color <= 0xFFFFFFFF); // "RGBA numeric value should be under 0xFFFFFFFF
    return {
        static_cast<uint8_t>((color & 0xFF000000) >> 24),
        static_cast<uint8_t>((color & 0xFF0000) >> 16),
        static_cast<uint8_t>((color & 0xFF00) >> 8),
        static_cast<uint8_t>(color & 0xFF)
    };
}

std::unique_ptr<GraphicsContext> GraphicsContext::create()
{
    return std::make_unique<SDLGraphicsContext>();
}
