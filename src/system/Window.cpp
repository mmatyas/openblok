#include "Window.h"

#include "GraphicsContext.h"
#include "sdl/SDLWindow.h"


std::unique_ptr<Window> Window::create()
{
    return std::make_unique<SDLWindow>();
}
