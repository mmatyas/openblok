#include "SDLEventCollector.h"

#include "SDL2/SDL.h"


SDLEventCollector::SDLEventCollector()
    : m_quit_requested(false)
{}

std::vector<InputEvent> SDLEventCollector::collect()
{
    std::vector<InputEvent> output;

    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
        case SDL_QUIT:
            m_quit_requested = true;
            break;
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            switch (sdl_event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
            case SDL_SCANCODE_F1:
            case SDL_SCANCODE_P:
                output.emplace_back(InputEvent(InputType::GAME_PAUSE, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_UP:
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
                output.emplace_back(InputEvent(InputType::GAME_HOLD, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDL_SCANCODE_SPACE:
                output.emplace_back(InputEvent(InputType::GAME_PAUSE, sdl_event.type == SDL_KEYDOWN));
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
