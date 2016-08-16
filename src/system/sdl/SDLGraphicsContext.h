#pragma once

#include "system/GraphicsContext.h"

#include "SDL2pp/SDL2pp.hh"
#include <string>
#include <unordered_map>


class SDLGraphicsContext: public GraphicsContext {
public:
    SDLGraphicsContext();

    void render() final;
    void toggleFullscreen() final;
    uint16_t screenWidth() final;
    uint16_t screenHeight() final;

    void loadFont(ResourceID, const std::string& path, unsigned pt) final;
    void cacheText(ResourceID, const std::string& text,
                   ResourceID font_id, const std::array<uint8_t, 4>& color) final;

    void drawTexture(ResourceID) final;

private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::SDLTTF ttf;

    std::unordered_map<ResourceID, std::unique_ptr<SDL2pp::Font>> fonts;
    std::unordered_map<ResourceID, std::unique_ptr<SDL2pp::Texture>> textures;
};
