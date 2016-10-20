#include "SDLWindow.h"

#include "SDLGraphicsContext.h"


SDLWindow::SDLWindow()
    : sdl(SDL_INIT_VIDEO)
    , window("OpenBlok",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        960, 720,
        SDL_WINDOW_RESIZABLE)
    , gcx(window)
    , m_quit_requested(false)
{
}

void SDLWindow::toggleFullscreen()
{
    window.SetFullscreen(window.GetFlags() ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void SDLWindow::requestScreenshot(const std::string& path)
{
    gcx.requestScreenshot(window, path);
}

void SDLWindow::setInputMapping(std::map<InputType, std::set<uint16_t>> mapping)
{
    for (const auto& elem : mapping) {
        for (const auto& scancode : elem.second)
            input_mapping[scancode] = elem.first;
    }
}

std::vector<InputEvent> SDLWindow::collectEvents()
{
    std::vector<InputEvent> output;

    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
        case SDL_QUIT:
            m_quit_requested = true;
            break;
        case SDL_WINDOWEVENT:
            switch (sdl_event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                m_quit_requested = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                output.emplace_back(InputEvent(InputType::GAME_PAUSE, true));
                break;
            case SDL_WINDOWEVENT_RESIZED:
                gcx.onResize(sdl_event.window.data1, sdl_event.window.data2);
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            case SDL_SCANCODE_F4:
                if (SDL_GetModState() & KMOD_ALT)
                    m_quit_requested = true;
            // do NOT break
        case SDL_KEYDOWN:
            if (!sdl_event.key.repeat) {
                uint16_t scancode = sdl_event.key.keysym.scancode;
                if (input_mapping.count(scancode))
                    output.emplace_back(InputEvent(input_mapping.at(scancode), sdl_event.type == SDL_KEYDOWN));
            }
        default:
            break;
        }
    }

    return output;
}
