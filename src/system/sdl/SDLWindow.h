#pragma once

#include "SDLGraphicsContext.h"
#include "system/GraphicsContext.h"
#include "system/Window.h"

#include "SDL2pp/SDL2pp.hh"
#include <memory>
#include <string>


class SDLWindow : public Window {
public:
    SDLWindow();

    void toggleFullscreen() final;
    void requestScreenshot(const std::string&) final;
    GraphicsContext& graphicsContext() { return gcx; };

    std::vector<InputEvent> collectEvents() final;
    bool quitRequested() final { return m_quit_requested; }

private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDLGraphicsContext gcx;

    bool m_quit_requested;
};
