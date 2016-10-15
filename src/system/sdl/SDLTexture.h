#pragma once

#include "system/Texture.h"

#include "SDL2pp/SDL2pp.hh"


class SDLTexture : public Texture {
public:
    SDLTexture(SDL2pp::Texture&&);

    void drawAt(int x, int y) final;
    void drawScaled(const Rectangle&) final;

    unsigned width() const final { return tex.GetWidth(); }
    unsigned height() const final { return tex.GetHeight(); }

private:
    static SDL2pp::Renderer* renderer;
    SDL2pp::Texture tex;

friend class SDLGraphicsContext;
};
