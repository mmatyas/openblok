#include "SDLTexture.h"

#include <assert.h>


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

void SDLTexture::drawPartialScaled(const Rectangle& from, const Rectangle& to)
{
    assert(renderer);
    renderer->Copy(tex,
                   SDL2pp::Rect(from.x, from.y, from.w, from.h),
                   SDL2pp::Rect(to.x, to.y, to.w, to.h));
}

void SDLTexture::setAlpha(uint8_t alpha)
{
    tex.SetAlphaMod(alpha);
}
