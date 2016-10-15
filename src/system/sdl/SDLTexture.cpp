#include "SDLTexture.h"


SDL2pp::Renderer* SDLTexture::renderer = nullptr;

SDLTexture::SDLTexture(SDL2pp::Texture&& tex)
    : tex(std::move(tex))
{}

void SDLTexture::drawAt(int x, int y)
{
    assert(renderer);
    renderer->Copy(tex, SDL2pp::NullOpt, SDL2pp::Point(x, y));
}

void SDLTexture::drawScaled(const Rectangle& rect)
{
    assert(renderer);
    renderer->Copy(tex, SDL2pp::NullOpt, SDL2pp::Rect(rect.x, rect.y, rect.w, rect.h));
}
