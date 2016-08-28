#include "SDLGraphicsContext.h"
#include "system/Log.h"

#include "SDL2/SDL.h"
#include "SDL2pp/SDL2pp.hh"
#include <exception>
#include <sstream>

using namespace SDL2pp;


const std::string LOG_TAG("video");

std::vector<std::string> splitByNL(const std::string& str) {
    std::vector<std::string> output;
    std::istringstream isst(str);
    std::string line;

    while (std::getline(isst, line))
        output.push_back(line);

    return output;
}

SDLGraphicsContext::SDLGraphicsContext()
    : sdl(SDL_INIT_VIDEO)
    , window("OpenBlok",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_RESIZABLE)
    , renderer(window, -1, SDL_RENDERER_ACCELERATED)
    , ttf()
    , on_render_callback([](){})
{
    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Clear();
    renderer.Present();

    Log::info(LOG_TAG) << screenWidth() << "x" << screenHeight() << " window created\n";
}

void SDLGraphicsContext::render()
{
    renderer.Present();
    on_render_callback();

    renderer.Clear();
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

void SDLGraphicsContext::loadFont(ResourceID slot, const std::string& path, unsigned pt)
{
    Log::info(LOG_TAG) << "Loading " << path << "\n";

    // emplace() returns an std::pair<Iter, bool>
    auto result = fonts.emplace(slot, std::make_unique<SDL2pp::Font>(path, pt));
    if (!result.second)
        throw std::runtime_error("Font already exists in slot " + std::to_string(slot));
}

void SDLGraphicsContext::cacheText(ResourceID slot, const std::string& text, ResourceID font_id, const std::array<uint8_t, 4>& color)
{
    if (!fonts.count(font_id))
        throw std::runtime_error("No font loaded in slot " + std::to_string(font_id));

    auto& font = fonts.at(font_id);
    auto lines = splitByNL(text);

    // shortcut for single lines
    if (lines.size() <= 1) {
        textures[slot] = std::make_unique<SDL2pp::Texture>(
            renderer,
            font->RenderUTF8_Blended(text, SDL_Color {color[0], color[1], color[2], color[3]})
        );
        return;
    }

    // find out texture dimensions
    int line_height = font->GetLineSkip();
    int width = 2; // to avoid zero size textures
    for (const std::string& line : lines) {
        auto line_width = font->GetSizeUTF8(line).GetX();
        if (line_width > width)
            width = line_width;
    }

    // to create an optimal base surface,
    // we are querying the current window's preferred pixel format
    auto pixelformat = SDL_GetWindowPixelFormat(window.Get());
    if (pixelformat == SDL_PIXELFORMAT_UNKNOWN)
        throw std::runtime_error(SDL_GetError());

    // extract the surface parameters from the SDL pixel format enum
    int bpp;
    Uint32 rmask, gmask, bmask, amask;
    if (!SDL_PixelFormatEnumToMasks(pixelformat, &bpp, &rmask, &gmask, &bmask, &amask))
        throw std::runtime_error(SDL_GetError());

    // create the surface and blit the lines on it
    SDL2pp::Surface basesurf(0, width,  line_height * lines.size(),
                             bpp, rmask, gmask, bmask, amask);
    for (unsigned l = 0; l < lines.size(); l++) {
        auto surf = font->RenderUTF8_Blended(lines.at(l), {color[0], color[1], color[2], color[3]});
        surf.Blit(NullOpt, basesurf, Rect(0, l * line_height, surf.GetWidth(), surf.GetHeight()));
    }

    textures[slot] = std::make_unique<SDL2pp::Texture>(renderer, basesurf);
}

void SDLGraphicsContext::loadTexture(ResourceID slot, const std::string& path)
{
    Log::info(LOG_TAG) << "Loading " << path << "\n";
    textures[slot] = std::make_unique<SDL2pp::Texture>(renderer, path);
}

void SDLGraphicsContext::loadTexture(ResourceID slot, const std::string& path, const std::array<uint8_t, 3>& color)
{
    loadTexture(slot, path);
    textures.at(slot)->SetColorMod(color[0], color[1], color[2]);
}

void SDLGraphicsContext::drawTexture(ResourceID slot, unsigned x, unsigned y)
{
    if (!textures.count(slot))
        throw std::runtime_error("No texture loaded in slot " + std::to_string(slot));

    auto& tex = textures.at(slot);
    renderer.Copy(*tex, NullOpt, Point(x, y));
}

void SDLGraphicsContext::requestScreenshot(const std::string& path)
{
    // TODO: if there'll be other callbacks, then this should be a FIFO list
    on_render_callback = [this, &path](){
        saveScreenshotBMP(path);
        on_render_callback = [](){};
    };
}

void SDLGraphicsContext::saveScreenshotBMP(const std::string& path)
{
    auto window_surface_raw = SDL_GetWindowSurface(window.Get());
    if (!window_surface_raw)
        throw std::runtime_error(SDL_GetError());

    SDL2pp::Surface info_surface(window_surface_raw);
    const auto info_format = info_surface.Get()->format;

    std::unique_ptr<uint8_t[]> pixels = std::make_unique<uint8_t[]>(
        info_surface.GetWidth() * info_surface.GetHeight() * info_format->BytesPerPixel);

    renderer.ReadPixels(
        info_surface.GetClipRect(),
        info_format->format,
        pixels.get(),
        info_surface.GetWidth() * info_format->BytesPerPixel);

    SDL2pp::Surface save_surface(pixels.get(),
        info_surface.GetWidth(),
        info_surface.GetHeight(),
        info_format->BitsPerPixel,
        info_surface.GetWidth() * info_format->BytesPerPixel,
        info_format->Rmask,
        info_format->Gmask,
        info_format->Bmask,
        info_format->Amask);

    SDL_SaveBMP(save_surface.Get(), path.c_str());
    Log::info(LOG_TAG) << "Screenshot saved to " << path << "\n";
}
