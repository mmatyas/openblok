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

    void loadFont(FontID, const std::string& path, unsigned pt) final;
    void cacheText(TexID, const std::string& text,
                   FontID, const RGBColor&) final;

    void loadTexture(TexID, const std::string& path) final;
    void loadTexture(TexID, const std::string& path, const RGBColor& tint) final;
    void drawTexture(TexID, unsigned x, unsigned y) final;
    void drawTexture(TexID, const Rectangle& rect) final;
    void drawFilledRect(const Rectangle& rect, const RGBColor& color) final;

    unsigned textureWidth(TexID) const final;
    unsigned textureHeight(TexID) const final;

    void requestScreenshot(const std::string& path) final;

private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::SDLTTF ttf;

    std::function<void()> on_render_callback;

    std::unordered_map<FontID, std::unique_ptr<SDL2pp::Font>, ResourceIDHash> fonts;
    std::unordered_map<TexID, std::unique_ptr<SDL2pp::Texture>, ResourceIDHash> textures;

    void saveScreenshotBMP(const std::string& path);
};
