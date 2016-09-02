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
            switch (sdl_event.key.keysym.sym) {
            case SDLK_UP:
                output.emplace_back(InputEvent(InputType::UP, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDLK_DOWN:
                output.emplace_back(InputEvent(InputType::DOWN, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDLK_LEFT:
                output.emplace_back(InputEvent(InputType::LEFT, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDLK_RIGHT:
                output.emplace_back(InputEvent(InputType::RIGHT, sdl_event.type == SDL_KEYDOWN));
                break;
            case SDLK_F4:
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
