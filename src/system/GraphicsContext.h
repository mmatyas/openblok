#pragma once

#include "ResourceUtils.h"

#include <array>
#include <memory>
#include <stdint.h>


class GraphicsContext {
public:
    virtual void render() = 0;
    virtual void toggleFullscreen() = 0;
    virtual uint16_t screenWidth() = 0;
    virtual uint16_t screenHeight() = 0;

    virtual void loadFont(ResourceID, const std::string& path, unsigned pt) = 0;
    virtual void cacheText(ResourceID, const std::string& text,
                           ResourceID font_id, const std::array<uint8_t, 4>& color) = 0;

    virtual void drawTexture(ResourceID) = 0;

private:
    static std::unique_ptr<GraphicsContext> create();

friend class AppContext;
};
