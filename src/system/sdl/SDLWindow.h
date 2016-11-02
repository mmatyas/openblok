#pragma once

#include "SDLAudioContext.h"
#include "SDLGraphicsContext.h"
#include "system/Window.h"

#include "SDL2pp/SDL2pp.hh"
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>


class SDLWindow : public Window {
public:
    SDLWindow();

    void toggleFullscreen() final;
    void requestScreenshot(const std::string&) final;
    GraphicsContext& graphicsContext() final { return gcx; };
    AudioContext& audioContext() final { return audio; };

    std::vector<InputEvent> collectEvents() final;
    void setInputMapping(std::map<InputType, std::vector<uint16_t>>) final;
    bool quitRequested() final { return m_quit_requested; }

private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDLGraphicsContext gcx;
    SDLAudioContext audio;
    std::unordered_map<SDL_JoystickID, std::unique_ptr<SDL_Joystick, std::function<void(SDL_Joystick*)>>> joysticks;

    std::unordered_map<uint8_t, std::set<InputType>> input_mapping;

    bool m_quit_requested;
};
