#include "GraphicsContext.h"

#include "sdl/SDLGraphicsContext.h"

std::unique_ptr<GraphicsContext> GraphicsContext::create()
{
    return std::make_unique<SDLGraphicsContext>();
}
