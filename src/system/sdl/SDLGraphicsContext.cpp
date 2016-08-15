#include "SDLGraphicsContext.h"
#include "system/Log.h"

#include "SDL2/SDL.h"
#include "SDL2pp/SDL2pp.hh"

using namespace SDL2pp;


SDLGraphicsContext::SDLGraphicsContext()
    : sdl(SDL_INIT_VIDEO)
    , window("OpenBlok",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_RESIZABLE)
    , renderer(window, -1, SDL_RENDERER_ACCELERATED)
{
    Log::info("video") << screenWidth() << "x" << screenHeight() << " window created\n";
}

void SDLGraphicsContext::toggleFullscreen()
{
    window.SetFullscreen(window.GetFlags() ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
}

uint16_t SDLGraphicsContext::screenWidth()
{
    return window.GetWidth();
}

uint16_t SDLGraphicsContext::screenHeight()
{
    return window.GetHeight();
}
