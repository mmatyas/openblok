#pragma once

#include "system/GraphicsContext.h"

#include "SDL2pp/SDL2pp.hh"
#include <string>
#include <unordered_map>

/// A graphics context implementation, based on SDL2
class SDLGraphicsContext: public GraphicsContext {
public:
    SDLGraphicsContext();

    void render() final;
    void toggleFullscreen() final;
    uint16_t screenWidth() const final;
    uint16_t screenHeight() const final;

    void loadFont(ResourceID, const std::string& path, unsigned pt) final;
    void cacheText(ResourceID, const std::string& text,
                   ResourceID font_id, const RGBColor& color) final;

    void loadTexture(ResourceID, const std::string&) final;
    void loadTexture(ResourceID, const std::string&, const RGBColor& color) final;
    void drawTexture(ResourceID, unsigned x, unsigned y) final;
    void drawTexture(ResourceID, const Rectangle& rect) final;
    void drawFilledRect(const Rectangle& rect, const RGBColor& color) final;

    unsigned textureWidth(ResourceID) const final;
    unsigned textureHeight(ResourceID) const final;

    void requestScreenshot(const std::string& path) final;

private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::SDLTTF ttf;

    std::function<void()> on_render_callback;

    std::unordered_map<ResourceID, std::unique_ptr<SDL2pp::Font>> fonts;
    std::unordered_map<ResourceID, std::unique_ptr<SDL2pp::Texture>> textures;

    void saveScreenshotBMP(const std::string& path);
};
