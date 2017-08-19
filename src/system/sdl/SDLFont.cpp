#include "SDLFont.h"

#include "SDLTexture.h"
#include "system/util/MakeUnique.h"

#include "SDL2/SDL.h"
#include <sstream>
#include <vector>
#include <assert.h>


SDL2pp::Renderer* SDLFont::renderer = nullptr;
uint32_t SDLFont::pixelformat = 0x0;

std::vector<std::string> splitByNL(const std::string& str) {
    std::vector<std::string> output;
    std::istringstream isst(str);
    std::string line;

    while (std::getline(isst, line))
        output.push_back(line);

    return output;
}

SDLFont::SDLFont(SDL2pp::Font&& font)
    : font(std::move(font))
{}

std::unique_ptr<Texture> SDLFont::renderText(const std::string& text, const RGBColor& color, TextAlign align)
{
    return renderText(text, RGBAColor {color.r, color.g, color.b, 255}, align);
}

std::unique_ptr<Texture> SDLFont::renderText(const std::string& text, const RGBAColor& color, TextAlign align)
{
    assert(renderer);

    const auto lines = splitByNL(text);

    // shortcut for single lines
    if (lines.size() <= 1) {
        return std::make_unique<SDLTexture>(SDL2pp::Texture(
            *renderer, font.RenderUTF8_Blended(text, {color.r, color.g, color.b, 255}).SetAlphaMod(color.a)
        ));
    }

    // find out texture dimensions
    const int line_height = font.GetLineSkip();
    int width = 2; // to avoid zero size textures
    for (const std::string& line : lines) {
        auto line_width = font.GetSizeUTF8(line).GetX();
        if (line_width > width)
            width = line_width;
    }

    // create the surface and blit the lines on it
    // NOTE: the setup of the surface (masks, initial fill) is based on
    // the implementation of TTF_RenderUTF8_Blended
    const SDL_Color rgba_color = {color.r, color.g, color.b, 255};
    SDL2pp::Surface basesurf(0x0, width, line_height * lines.size(),
                             32, 0xff0000, 0xff00, 0xff, 0xff000000);
    basesurf.FillRect(SDL2pp::NullOpt, (color.r << 16) | (color.g << 8) | color.b);

    switch (align) {
        case TextAlign::LEFT:
            for (unsigned l = 0; l < lines.size(); l++) {
                auto surf = font.RenderUTF8_Blended(lines.at(l), rgba_color);
                surf.Blit(SDL2pp::NullOpt,
                          basesurf,
                          SDL2pp::Rect(0, l * line_height, surf.GetWidth(), surf.GetHeight()));
            }
            break;
        case TextAlign::RIGHT:
            for (unsigned l = 0; l < lines.size(); l++) {
                auto surf = font.RenderUTF8_Blended(lines.at(l), rgba_color);
                surf.Blit(SDL2pp::NullOpt,
                          basesurf,
                          SDL2pp::Rect(basesurf.GetWidth() - surf.GetWidth(), l * line_height,
                                       surf.GetWidth(), surf.GetHeight()));
            }
            break;
        default:
            assert(false);
    }

    return std::make_unique<SDLTexture>(SDL2pp::Texture(*renderer, basesurf.SetAlphaMod(color.a)));
}
