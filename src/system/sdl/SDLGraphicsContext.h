#pragma once

#include "system/GraphicsContext.h"

#include <SDL2pp/SDL2pp.hh>
#include <map>


/// A graphics context implementation, based on SDL2
class SDLGraphicsContext: public GraphicsContext {
public:
    SDLGraphicsContext(SDL2pp::Window&);
    ~SDLGraphicsContext();

    void render() final;
    unsigned short screenWidth() const final;
    unsigned short screenHeight() const final;

    float getDrawScale() const final;
    void modifyDrawScale(float scale) final;

    std::shared_ptr<Font> loadFont(const std::string& path, unsigned pt) final;
    std::unique_ptr<Texture> loadTexture(const std::string& path) final;
    std::unique_ptr<Texture> loadTexture(const std::string& path, const RGBColor& tint) final;

    void drawFilledRect(const Rectangle& rect, const RGBColor& color) final;
    void drawFilledRect(const Rectangle& rect, const RGBAColor& color) final;

    // SDL only
    void requestScreenshot(const SDL2pp::Window&, const std::string& path);
    void onResize(int width, int height);

private:
    SDL2pp::Renderer renderer;
    SDL2pp::SDLImage image_loader;
    SDL2pp::SDLTTF ttf;
    uint32_t pixelformat;

    std::map<std::string, std::shared_ptr<Font>> font_cache;

    std::function<void()> on_render_callback;
    void saveScreenshotBMP(const SDL2pp::Window&, const std::string& path);
};
