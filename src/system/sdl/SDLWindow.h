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
    bool quitRequested() final { return m_quit_requested; }

    void setInputConfig(const std::map<DeviceName, DeviceData>&) final;
    void setKeyBinding(DeviceID, InputType, uint16_t) final;
    std::map<DeviceName, DeviceData> createInputConfig() const final;
    const DeviceMap& connectedDevices() const final;
    std::string buttonName(DeviceID, uint16_t) const final;

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

    std::map<DeviceName, DeviceData> known_mappings; ///< all known device mappings, but without IDs
    ButtonToEventsMap knownButtonmapForDeviceName(const std::string&);

    DeviceMap device_maps; ///< the input maps of all connected devices
    const ButtonToEventsMap default_keyboard_mapping;
    const ButtonToEventsMap default_gamepad_mapping;
    const ButtonToEventsMap default_joystick_mapping;

    bool m_quit_requested;
};
