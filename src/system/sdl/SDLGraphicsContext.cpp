#include "SDLGraphicsContext.h"
#include "system/Log.h"

#include "SDL2/SDL.h"
#include "SDL2pp/SDL2pp.hh"
#include <exception>

using namespace SDL2pp;


const std::string LOG_TAG("video");

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
    auto result = textures.emplace(slot, std::make_unique<SDL2pp::Texture>(
        renderer,
        font->RenderUTF8_Blended(text, SDL_Color {color[0], color[1], color[2], color[3]})
    ));
    if (!result.second)
        throw std::runtime_error("Cached text already exists in slot " + std::to_string(slot));
}

void SDLGraphicsContext::drawTexture(ResourceID slot)
{
    if (!textures.count(slot))
        throw std::runtime_error("No texture loaded in slot " + std::to_string(slot));

    auto& tex = textures.at(slot);
    renderer.Copy(*tex, NullOpt, Rect(0, 0, tex->GetWidth(), tex->GetHeight()));
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
    SDL2pp::Surface info_surface = SDL_GetWindowSurface(window.Get());
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
