#include "SDLWindow.h"

#include "version.h"
#include "SDLGraphicsContext.h"
#include "system/Log.h"
#include "system/Paths.h"

#include <assert.h>


const std::string LOG_INPUT_TAG = "input";

SDLWindow::SDLWindow()
    : sdl(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER)
    , window(std::string("OpenBlok ") + game_version,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        960, 540,
        SDL_WINDOW_RESIZABLE)
    , gcx(window)
    , default_keyboard_mapping({
            {SDL_SCANCODE_P, {InputType::GAME_PAUSE}},
            {SDL_SCANCODE_C, {InputType::GAME_HOLD}},
            {SDL_SCANCODE_LSHIFT, {InputType::GAME_HARDDROP}},
            {SDL_SCANCODE_RSHIFT, {InputType::GAME_HARDDROP}},
            {SDL_SCANCODE_SPACE, {InputType::GAME_HARDDROP, InputType::MENU_OK}},
            {SDL_SCANCODE_UP, {InputType::GAME_HARDDROP, InputType::MENU_UP}},
            {SDL_SCANCODE_DOWN, {InputType::GAME_SOFTDROP, InputType::MENU_DOWN}},
            {SDL_SCANCODE_LEFT, {InputType::GAME_MOVE_LEFT, InputType::MENU_LEFT}},
            {SDL_SCANCODE_RIGHT, {InputType::GAME_MOVE_RIGHT, InputType::MENU_RIGHT}},
            {SDL_SCANCODE_Z, {InputType::GAME_ROTATE_LEFT, InputType::MENU_OK}},
            {SDL_SCANCODE_X, {InputType::GAME_ROTATE_RIGHT, InputType::MENU_CANCEL}},
            {SDL_SCANCODE_RETURN, {InputType::MENU_OK}},
            {SDL_SCANCODE_BACKSPACE, {InputType::MENU_CANCEL}},
            {SDL_SCANCODE_ESCAPE, {InputType::MENU_CANCEL}},
        })
    , default_gamepad_mapping({
            {SDL_CONTROLLER_BUTTON_DPAD_UP, {InputType::GAME_HARDDROP, InputType::MENU_UP}},
            {SDL_CONTROLLER_BUTTON_DPAD_DOWN, {InputType::GAME_SOFTDROP, InputType::MENU_DOWN}},
            {SDL_CONTROLLER_BUTTON_DPAD_LEFT, {InputType::GAME_MOVE_LEFT, InputType::MENU_LEFT}},
            {SDL_CONTROLLER_BUTTON_DPAD_RIGHT, {InputType::GAME_MOVE_RIGHT, InputType::MENU_RIGHT}},
            {SDL_CONTROLLER_BUTTON_A, {InputType::GAME_ROTATE_LEFT, InputType::MENU_OK}},
            {SDL_CONTROLLER_BUTTON_B, {InputType::GAME_ROTATE_RIGHT, InputType::MENU_CANCEL}},
            {SDL_CONTROLLER_BUTTON_LEFTSHOULDER, {InputType::GAME_HOLD}},
            {SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, {InputType::GAME_HOLD}},
            {SDL_CONTROLLER_BUTTON_BACK, {InputType::GAME_PAUSE}},
            {SDL_CONTROLLER_BUTTON_GUIDE, {InputType::GAME_PAUSE}},
            {SDL_CONTROLLER_BUTTON_START, {InputType::GAME_PAUSE}},
        })
    // In SDL, the legacy joystick API uses 1 byte for button indices,
    // and also 1 byte for hats, both starting from 0. Here we store
    // them in the same map using 2 bytes, buttons in the upper byte
    // with 0xFF below, and hats in the lower byte.
    , default_joystick_mapping({
            {SDL_HAT_UP, {InputType::GAME_HARDDROP, InputType::MENU_UP}},
            {SDL_HAT_DOWN, {InputType::GAME_SOFTDROP, InputType::MENU_DOWN}},
            {SDL_HAT_LEFT, {InputType::GAME_MOVE_LEFT, InputType::MENU_LEFT}},
            {SDL_HAT_RIGHT, {InputType::GAME_MOVE_RIGHT, InputType::MENU_RIGHT}},
            {0 + 0xFF, {InputType::GAME_ROTATE_LEFT}},
            {(1 << 8) + 0xFF, {InputType::GAME_ROTATE_LEFT, InputType::MENU_OK}},
            {(2 << 8) + 0xFF, {InputType::GAME_ROTATE_RIGHT, InputType::MENU_CANCEL}},
            {(3 << 8) + 0xFF, {InputType::GAME_ROTATE_RIGHT}},
            {(4 << 8) + 0xFF, {InputType::GAME_HOLD}},
            {(5 << 8) + 0xFF, {InputType::GAME_HOLD}},
            {(8 << 8) + 0xFF, {InputType::GAME_PAUSE}},
            {(9 << 8) + 0xFF, {InputType::GAME_PAUSE}},
        })
    , m_quit_requested(false)
{
    window.SetIcon(SDL2pp::Surface(Paths::data() + "icon.png"));
    SDL_GameControllerAddMappingsFromFile((Paths::data() + "gamecontrollerdb").c_str());
    device_names[-1] = "keyboard";
    device_types[-1] = DeviceType::KEYBOARD;
    device_maps[-1] = default_keyboard_mapping;
}

void SDLWindow::toggleFullscreen()
{
    window.SetFullscreen(window.GetFlags() ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void SDLWindow::requestScreenshot(const std::string& path)
{
    gcx.requestScreenshot(window, path);
}

void SDLWindow::setKnownInputMappings(const DeviceMap& devices)
{
    known_mappings = devices;
    auto& device_map = device_maps[-1];
    device_map = mapForDeviceName(device_names.at(-1));
    if (device_map.empty())
        device_map = default_keyboard_mapping;
}

DeviceMap SDLWindow::inputMappings() const
{
    DeviceMap output = known_mappings;
    for (const auto& device : device_maps) {
        DeviceData device_data;
        device_data.type = device_types.at(device.first);

        // Convert ButtonsToEventsMap to EventsToButtonsMap
        for (const auto& buttonmap : device.second) { // pair <button, [events]>
            for (const auto& event : buttonmap.second)
                device_data.eventmap[event].emplace_back(buttonmap.first);
        }

        const auto& device_name = device_names.at(device.first);
        output[device_name] = device_data;
    }
    return output;
}

ButtonToEventsMap SDLWindow::mapForDeviceName(const std::string& device_name)
{
    ButtonToEventsMap button_map;
    if (!known_mappings.count(device_name))
        return button_map;

    // Convert EventsToButtonsMap to ButtonsToEventsMap
    const EventToButtonsMap& event_map = known_mappings.at(device_name).eventmap;
    for (const auto& curr_event : event_map) { // pair <event, [buttons]>
        for (const auto& button : curr_event.second)
            button_map[button].emplace_back(curr_event.first);
    }
    return button_map;
}

std::vector<Event> SDLWindow::collectEvents()
{
    /* Note: because the SDL2 GameController API builds on top the SDL Joystick API,
     * game controllers can also receive joaystick events.
     * There are `if (joysticks.count(..))` checks at the switch, to avoid running
     * the input handling code for both APIs.
     */

    std::vector<Event> output;

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
                    output.emplace_back(WindowEvent::FOCUS_LOST);
                    break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    output.emplace_back(WindowEvent::FOCUS_GAINED);
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    gcx.onResize(sdl_event.window.data1, sdl_event.window.data2);
                    output.emplace_back(WindowEvent::RESIZED);
                    break;
                default:
                    break;
            }
            break;
        case SDL_CONTROLLERDEVICEADDED:
            // Note: It seems this event doesn't always trigger,
            // so the code was moved to SDL_JOYDEVICEADDED, which happens
            // for both GameControllers and Joysticks.
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            if (gamepads.count(sdl_event.cdevice.which)) {
                Log::info(LOG_INPUT_TAG) << "Gamepad disconnected: "
                                         << SDL_GameControllerName(gamepads.at(sdl_event.cdevice.which).get()) << "\n";
                gamepads.erase(sdl_event.cdevice.which);
            }
            break;
        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN: {
                assert(gamepads.count(sdl_event.cbutton.which));
                assert(device_maps.count(sdl_event.cbutton.which));
                auto& device_map = device_maps.at(sdl_event.cbutton.which);
                for (const auto& input_event : device_map[sdl_event.cbutton.button])
                    output.emplace_back(InputEvent(input_event, sdl_event.type == SDL_CONTROLLERBUTTONDOWN));
            }
            break;
        case SDL_JOYDEVICEADDED:
            if (SDL_IsGameController(sdl_event.jdevice.which)) {
                auto gamepad = std::unique_ptr<SDL_GameController, std::function<void(SDL_GameController*)>>(
                    SDL_GameControllerOpen(sdl_event.jdevice.which),
                    [](SDL_GameController* ptr){
                        if (SDL_GameControllerGetAttached(ptr)) SDL_GameControllerClose(ptr); });

                if (gamepad) {
                    auto joystick = SDL_GameControllerGetJoystick(gamepad.get());
                    auto iid = SDL_JoystickInstanceID(joystick); // returns negative on error
                    if (iid >= 0) {
                        const std::string name = SDL_GameControllerName(gamepad.get());
                        gamepads[iid] = std::move(gamepad);
                        auto& device_map = device_maps[iid];
                        if (device_map.empty())
                            device_map = mapForDeviceName(name);
                        if (device_map.empty())
                            device_map = default_gamepad_mapping;
                        device_names[iid] = name;
                        device_types[iid] = DeviceType::GAMEPAD;
                        Log::info(LOG_INPUT_TAG) << "Gamepad connected: " <<  name << "\n";
                    }
                }
            }
            else {
                auto joy = std::unique_ptr<SDL_Joystick, std::function<void(SDL_Joystick*)>>(
                    SDL_JoystickOpen(sdl_event.jdevice.which),
                    [](SDL_Joystick* ptr){
                        if (SDL_JoystickGetAttached(ptr)) SDL_JoystickClose(ptr); });

                if (joy) {
                    auto iid = SDL_JoystickInstanceID(joy.get()); // returns negative on error
                    if (iid >= 0) {
                        const std::string name = SDL_JoystickName(joy.get());
                        joysticks[iid] = std::move(joy);
                        auto& device_map = device_maps[iid];
                        if (device_map.empty())
                            device_map = mapForDeviceName(name);
                        if (device_map.empty())
                            device_map = default_joystick_mapping;
                        device_names[iid] = name;
                        device_types[iid] = DeviceType::LEGACY_JOYSTICK;
                        Log::info(LOG_INPUT_TAG) << "Joystick connected: " << name << "\n";
                    }
                }
            }
            break;
        case SDL_JOYDEVICEREMOVED:
            if (joysticks.count(sdl_event.jdevice.which)) {
                Log::info(LOG_INPUT_TAG) << "Joystick disconnected: "
                                         << SDL_JoystickName(joysticks.at(sdl_event.jdevice.which).get()) << "\n";
                joysticks.erase(sdl_event.jdevice.which);
            }
            break;
        case SDL_JOYHATMOTION:
            if (joysticks.count(sdl_event.jhat.which)) {
                assert(device_maps.count(sdl_event.jhat.which));
                auto& device_map = device_maps.at(sdl_event.jhat.which);
                uint16_t button = sdl_event.jhat.value;
                // turn off all hat keys - there can be only one direction active at a time
                static const auto all_hats = {SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT};
                for (const auto& hat : all_hats) {
                    for (const auto& event : device_map[hat])
                        output.emplace_back(InputEvent(event, false));
                }
                // turn on only the current one
                for (const auto& event : device_map[button])
                    output.emplace_back(InputEvent(event, true));
            }
            break;
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
            if (joysticks.count(sdl_event.jbutton.which)) {
                assert(device_maps.count(sdl_event.jbutton.which));
                auto& device_map = device_maps.at(sdl_event.jbutton.which);
                // reminder: buttons are stored on the upper byte
                uint16_t button = (sdl_event.jbutton.button << 8) + 0xFF;
                for (const auto& input_event : device_map[button])
                    output.emplace_back(InputEvent(input_event, sdl_event.type == SDL_JOYBUTTONDOWN));
            }
            break;
        case SDL_KEYUP:
            if (sdl_event.key.keysym.scancode == SDL_SCANCODE_F4 && SDL_GetModState() & KMOD_ALT)
                m_quit_requested = true;
            // do NOT break - the code below should run for KEYUP too
        case SDL_KEYDOWN:
            if (!sdl_event.key.repeat) {
                assert(device_maps.count(-1));
                auto& device_map = device_maps.at(-1);
                uint16_t scancode = sdl_event.key.keysym.scancode;
                if (device_map.count(scancode)) {
                    for (const auto& input_event : device_map.at(scancode))
                        output.emplace_back(InputEvent(input_event, sdl_event.type == SDL_KEYDOWN));
                }
            }
            break;
        default:
            break;
        }
    }

    return output;
}

void SDLWindow::showErrorMessage(const std::string& title, const std::string& content)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), content.c_str(), NULL);
}
