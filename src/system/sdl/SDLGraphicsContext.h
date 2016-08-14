#pragma once

#include "../GraphicsContext.h"

#include "SDL2pp/SDL2pp.hh"


class SDLGraphicsContext: public GraphicsContext {
public:
    SDLGraphicsContext();

    void toggleFullscreen() final;
    uint16_t screenWidth() final;
    uint16_t screenHeight() final;

private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;

    bool is_fullscreen;
};
