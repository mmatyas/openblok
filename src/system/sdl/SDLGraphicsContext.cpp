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

SDLGraphicsContext::SDLGraphicsContext(SDL2pp::Window& window)
    : renderer(window, -1, SDL_RENDERER_ACCELERATED)
    , ttf()
    , on_render_callback([](){})
    , current_fontid(0)
    , current_texid(0)
{
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    onResize(window.GetWidth(), window.GetHeight());

    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Clear();
    renderer.Present();

    // save the window's preferred pixel format for optimal drawing later
    pixelformat = SDL_GetWindowPixelFormat(window.Get());
    if (pixelformat == SDL_PIXELFORMAT_UNKNOWN)
        throw std::runtime_error(SDL_GetError());

    Log::info(LOG_TAG) << screenWidth() << "x" << screenHeight() << " window created\n";
}

void SDLGraphicsContext::render()
{
    renderer.Present();
    on_render_callback();

    renderer.Clear();
}

uint16_t SDLGraphicsContext::screenWidth() const
{
    return renderer.GetLogicalWidth();
}

uint16_t SDLGraphicsContext::screenHeight() const
{
    return renderer.GetLogicalHeight();
}

FontID SDLGraphicsContext::loadFont(const std::string& path, unsigned pt)
{
    Log::info(LOG_TAG) << "Loading " << path << "\n";

    // emplace() returns an std::pair<Iter, bool>
    auto result = fonts.emplace(current_fontid, std::make_unique<SDL2pp::Font>(path, pt));
    if (!result.second)
        throw std::runtime_error("Font already exists in slot " + std::to_string(current_fontid));

    return current_fontid++;
}

TextureID SDLGraphicsContext::renderText(const std::string& text, FontID font_id, const RGBColor& color)
{
    renderText(current_texid, text, font_id, color);
    return current_texid++;
}

void SDLGraphicsContext::renderText(TextureID target_slot, const std::string& text,
                                    FontID font_id, const RGBColor& color)
{
    if (!fonts.count(font_id))
        throw std::runtime_error("No font loaded in slot " + std::to_string(font_id));

    auto& font = fonts.at(font_id);
    auto lines = splitByNL(text);

    // shortcut for single lines
    if (lines.size() <= 1) {
        textures[target_slot] = std::make_unique<SDL2pp::Texture>(
            renderer,
            font->RenderUTF8_Blended(text, {color.r, color.g, color.b, 255})
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

    // extract the surface parameters from the SDL pixel format enum
    int bpp;
    Uint32 rmask, gmask, bmask, amask;
    if (!SDL_PixelFormatEnumToMasks(pixelformat, &bpp, &rmask, &gmask, &bmask, &amask))
        throw std::runtime_error(SDL_GetError());

    // create the surface and blit the lines on it
    SDL2pp::Surface basesurf(0, width,  line_height * lines.size(),
                             bpp, rmask, gmask, bmask, amask);
    for (unsigned l = 0; l < lines.size(); l++) {
        auto surf = font->RenderUTF8_Blended(lines.at(l), {color.r, color.g, color.b, 255});
        surf.Blit(NullOpt, basesurf, Rect(0, l * line_height, surf.GetWidth(), surf.GetHeight()));
    }

    textures[target_slot] = std::make_unique<SDL2pp::Texture>(renderer, basesurf);
}

TextureID SDLGraphicsContext::loadTexture(const std::string& path)
{
    Log::info(LOG_TAG) << "Loading " << path << "\n";
    textures[current_texid] = std::make_unique<SDL2pp::Texture>(renderer, path);
    return current_texid++;
}

TextureID SDLGraphicsContext::loadTexture(const std::string& path, const RGBColor& tint)
{
    TextureID texid = loadTexture(path);
    textures.at(texid)->SetColorMod(tint.r, tint.g, tint.b);
    return texid;
}

void SDLGraphicsContext::unloadTexture(TextureID slot)
{
    textures.erase(slot);
}

void SDLGraphicsContext::unloadFont(FontID slot)
{
    fonts.erase(slot);
}

void SDLGraphicsContext::drawTexture(TextureID slot, unsigned x, unsigned y)
{
    if (!textures.count(slot))
        throw std::runtime_error("No texture loaded in slot " + std::to_string(slot));

    renderer.Copy(*textures.at(slot), NullOpt, Point(x, y));
}

void SDLGraphicsContext::drawTexture(TextureID slot, const Rectangle& rect)
{
    if (!textures.count(slot))
        throw std::runtime_error("No texture loaded in slot " + std::to_string(slot));

    renderer.Copy(*textures.at(slot), NullOpt, Rect(rect.x, rect.y, rect.w, rect.h));
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

unsigned SDLGraphicsContext::textureWidth(TextureID slot) const
{
    return textures.at(slot)->GetWidth();
}

unsigned SDLGraphicsContext::textureHeight(TextureID slot) const
{
    return textures.at(slot)->GetHeight();
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
