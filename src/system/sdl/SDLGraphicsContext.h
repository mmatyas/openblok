#pragma once

#include "system/GraphicsContext.h"

#include "SDL2pp/SDL2pp.hh"
#include <string>
#include <unordered_map>


/// A graphics context implementation, based on SDL2
class SDLGraphicsContext: public GraphicsContext {
public:
    SDLGraphicsContext(SDL2pp::Window&);

    void render() final;
    uint16_t screenWidth() const final;
    uint16_t screenHeight() const final;

    FontID loadFont(const std::string& path, unsigned pt) final;
    void unloadFont(FontID) final;
    TextureID renderText(const std::string& text,
                         FontID font, const RGBColor&) final;
    void renderText(TextureID target_slot, const std::string& text,
                    FontID font, const RGBColor&) final;

    TextureID loadTexture(const std::string& path) final;
    TextureID loadTexture(const std::string& path, const RGBColor& tint) final;
    void unloadTexture(TextureID) final;
    void drawTexture(TextureID, unsigned x, unsigned y) final;
    void drawTexture(TextureID, const Rectangle& rect) final;
    void drawFilledRect(const Rectangle& rect, const RGBColor& color) final;
    void drawFilledRect(const Rectangle& rect, const RGBAColor& color) final;

    unsigned textureWidth(TextureID) const final;
    unsigned textureHeight(TextureID) const final;

    // SDL only
    void requestScreenshot(const SDL2pp::Window&, const std::string& path);
    void onResize(int width, int height);

private:
    SDL2pp::Renderer renderer;
    SDL2pp::SDLTTF ttf;
    uint32_t pixelformat;

    std::function<void()> on_render_callback;

    FontID current_fontid;
    TextureID current_texid;
    std::unordered_map<FontID, std::unique_ptr<SDL2pp::Font>> fonts;
    std::unordered_map<TextureID, std::unique_ptr<SDL2pp::Texture>> textures;

    void saveScreenshotBMP(const SDL2pp::Window&, const std::string& path);
};
