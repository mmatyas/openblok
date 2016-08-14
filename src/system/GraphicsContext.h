#pragma once

#include <memory>
#include <stdint.h>

class GraphicsContext {
public:
    static std::unique_ptr<GraphicsContext> create();

    virtual void toggleFullscreen() = 0;
    virtual uint16_t screenWidth() = 0;
    virtual uint16_t screenHeight() = 0;
};
