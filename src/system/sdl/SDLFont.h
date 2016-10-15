#pragma once

#include "system/Font.h"

#include "SDL2pp/SDL2pp.hh"


class SDLFont : public Font {
public:
    SDLFont(SDL2pp::Font&&);
    std::unique_ptr<Texture> renderText(const std::string&, const RGBColor&) final;

private:
    static SDL2pp::Renderer* renderer;
    static uint32_t pixelformat;
    SDL2pp::Font font;

friend class SDLGraphicsContext;
};
