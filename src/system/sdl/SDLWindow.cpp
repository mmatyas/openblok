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
        case SDL_KEYDOWN:
            switch (sdl_event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
            case SDL_SCANCODE_F1:
            case SDL_SCANCODE_P:
                if (!sdl_event.key.repeat)
                    output.emplace_back(InputEvent(InputType::GAME_PAUSE, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_UP:
                if (!sdl_event.key.repeat)
                    output.emplace_back(InputEvent(InputType::UP, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_DOWN:
                output.emplace_back(InputEvent(InputType::DOWN, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_LEFT:
                output.emplace_back(InputEvent(InputType::LEFT, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_RIGHT:
                output.emplace_back(InputEvent(InputType::RIGHT, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_Z:
                output.emplace_back(InputEvent(InputType::A, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_X:
                output.emplace_back(InputEvent(InputType::B, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_C:
            case SDL_SCANCODE_LSHIFT:
            case SDL_SCANCODE_RSHIFT:
                if (!sdl_event.key.repeat)
                    output.emplace_back(InputEvent(InputType::GAME_HOLD, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_SPACE:
                if (!sdl_event.key.repeat)
                    output.emplace_back(InputEvent(InputType::GAME_HARDDROP, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_F4:
                if (SDL_GetModState() & KMOD_ALT)
                    m_quit_requested = true;
                break;
            default:
                break;
            }
        default:
            break;
        }
    }

    return output;
}
