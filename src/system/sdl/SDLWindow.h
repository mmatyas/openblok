#pragma once

#include "SDLAudioContext.h"
#include "SDLGraphicsContext.h"
#include "system/Window.h"

#include <SDL2pp/SDL2pp.hh>
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

    std::vector<Event> collectEvents() final;
    void setKnownInputMappings(const DeviceMap&) final;
    DeviceMap inputMappings() const final;
    bool quitRequested() final { return m_quit_requested; }

    static void showErrorMessage(const std::string& title, const std::string& content);

private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDLGraphicsContext gcx;
    SDLAudioContext audio;
    std::unordered_map<SDL_JoystickID,
        std::unique_ptr<SDL_Joystick, std::function<void(SDL_Joystick*)>>> joysticks;
    std::unordered_map<SDL_JoystickID,
        std::unique_ptr<SDL_GameController, std::function<void(SDL_GameController*)>>> gamepads;

    DeviceMap known_mappings; ///< names -> |type, [event, [buttons]]|
    ButtonToEventsMap mapForDeviceName(const std::string&);

    std::unordered_map<SDL_JoystickID, ButtonToEventsMap> device_maps;
    std::unordered_map<SDL_JoystickID, DeviceName> device_names;
    std::unordered_map<SDL_JoystickID, DeviceType> device_types;
    const ButtonToEventsMap default_keyboard_mapping;
    const ButtonToEventsMap default_gamepad_mapping;
    const ButtonToEventsMap default_joystick_mapping;

    bool m_quit_requested;
};
