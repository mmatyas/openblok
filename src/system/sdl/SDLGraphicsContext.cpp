#include "SDLGraphicsContext.h"

#include "SDLFont.h"
#include "SDLTexture.h"
#include "system/Log.h"

#include "SDL2pp/SDL2pp.hh"
#include <exception>


const std::string LOG_TAG("video");

SDLGraphicsContext::SDLGraphicsContext(SDL2pp::Window& window)
    : renderer(window, -1, 0x0)
    , ttf()
    , on_render_callback([](){})
{
    SDL_RendererInfo rinfo;
    renderer.GetInfo(rinfo);
    Log::info(LOG_TAG) << "Selected '" << rinfo.name << "' renderer\n";

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    onResize(window.GetWidth(), window.GetHeight());

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Clear();
    renderer.Present();

    // save the window's preferred pixel format for optimal drawing later
    pixelformat = SDL_GetWindowPixelFormat(window.Get());
    if (pixelformat == SDL_PIXELFORMAT_UNKNOWN)
        throw std::runtime_error(SDL_GetError());

    SDLTexture::renderer = &renderer;
    SDLFont::pixelformat = pixelformat;
    SDLFont::renderer = &renderer;
}

SDLGraphicsContext::~SDLGraphicsContext()
{
    SDLTexture::renderer = nullptr;
    SDLFont::renderer = nullptr;
}

void SDLGraphicsContext::render()
{
    renderer.Present();
    on_render_callback();

    renderer.Clear();
}

unsigned short SDLGraphicsContext::screenWidth() const
{
    return renderer.GetLogicalWidth();
}

unsigned short SDLGraphicsContext::screenHeight() const
{
    return renderer.GetLogicalHeight();
}

float SDLGraphicsContext::getDrawScale() const
{
    float x, y;
    renderer.GetScale(x, y);
    assert(std::abs(x - y) < 0.001);
    return x;
}

void SDLGraphicsContext::modifyDrawScale(float scale)
{
    renderer.SetScale(scale, scale);
}

std::shared_ptr<Font> SDLGraphicsContext::loadFont(const std::string& path, unsigned pt)
{
    return std::make_shared<SDLFont>(SDL2pp::Font(path, pt));
}

std::unique_ptr<Texture> SDLGraphicsContext::loadTexture(const std::string& path)
{
    return std::make_unique<SDLTexture>(SDL2pp::Texture(renderer, path));
}

std::unique_ptr<Texture> SDLGraphicsContext::loadTexture(const std::string& path, const RGBColor& tint)
{
    SDL2pp::Texture tex(renderer, path);
    tex.SetColorMod(tint.r, tint.g, tint.b);
    return std::make_unique<SDLTexture>(std::move(tex));
}

void SDLGraphicsContext::drawFilledRect(const Rectangle& rect, const RGBColor& color)
{
    Uint8 r, g, b, a;
    renderer.GetDrawColor(r, g, b, a);
    renderer.SetDrawColor(color.r, color.g, color.b);
    renderer.FillRect(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
    renderer.SetDrawColor(r, g, b, a);
}

void SDLGraphicsContext::drawFilledRect(const Rectangle& rect, const RGBAColor& color)
{
    // TODO: fix duplication
    Uint8 r, g, b, a;
    auto blend = renderer.GetDrawBlendMode();
    renderer.GetDrawColor(r, g, b, a);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(color.r, color.g, color.b, color.a);
    renderer.FillRect(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);

    renderer.SetDrawBlendMode(blend);
    renderer.SetDrawColor(r, g, b, a);
}

void SDLGraphicsContext::requestScreenshot(const SDL2pp::Window& window, const std::string& path)
{
    // TODO: if there'll be other callbacks, then this should be a FIFO list
    on_render_callback = [this, &window, &path](){
        saveScreenshotBMP(window, path);
        on_render_callback = [](){};
    };
}

void SDLGraphicsContext::onResize(int width, int height)
{
    static constexpr float min_logical_w = 960;
    static constexpr float min_logical_h = 720;
    static constexpr float min_aspect_ratio = min_logical_w / min_logical_h;

    const float width_ratio = width / min_logical_w;
    const float height_ratio = height / min_logical_h;
    const float window_aspect_ratio = (float) width / height;

    int logical_width = min_logical_w;
    int logical_height = min_logical_h;
    if (window_aspect_ratio > min_aspect_ratio) {
        // wider than 4:3
        logical_width *= width / (min_logical_w * height_ratio);
    }
    else {
        // taller than 4:3
        logical_height *= height / (min_logical_h * width_ratio);
    }

    renderer.SetLogicalSize(logical_width, logical_height);
}

void SDLGraphicsContext::saveScreenshotBMP(const SDL2pp::Window& window, const std::string& path)
{
    auto window_surface_raw = SDL_GetWindowSurface(window.Get());
    if (!window_surface_raw)
        throw std::runtime_error(SDL_GetError());

    const SDL2pp::Surface info_surface(window_surface_raw);
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
