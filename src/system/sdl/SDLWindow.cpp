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
            {SDL_SCANCODE_ESCAPE, {InputType::GAME_PAUSE, InputType::MENU_CANCEL}},
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

    device_maps[-1].id = -1;
    device_maps.at(-1).name = "keyboard";
    device_maps.at(-1).type = DeviceType::KEYBOARD;
    device_maps.at(-1).buttonmap = default_keyboard_mapping;
    device_maps.at(-1).eventmap = toEventMap(device_maps.at(-1).buttonmap);

    SDL_ShowCursor(SDL_DISABLE);
}

void SDLWindow::toggleFullscreen()
{
    window.SetFullscreen(window.GetFlags() ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void SDLWindow::requestScreenshot(const std::string& path)
{
    gcx.requestScreenshot(window, path);
}

void SDLWindow::setInputConfig(const std::map<DeviceName, DeviceData>& known)
{
    known_mappings = known;
    auto& kb_buttonmap = device_maps.at(-1).buttonmap;
    kb_buttonmap = knownButtonmapForDeviceName(device_maps.at(-1).name);
    if (kb_buttonmap.empty())
        kb_buttonmap = default_keyboard_mapping;
    device_maps.at(-1).eventmap = toEventMap(kb_buttonmap);
}

std::map<DeviceName, DeviceData> SDLWindow::createInputConfig() const
{
    std::map<DeviceName, DeviceData> output = known_mappings;
    for (const auto& device : device_maps)
        output[device.second.name] = device.second;
    return output;
}

const DeviceMap& SDLWindow::connectedDevices() const
{
    return device_maps;
}

std::string SDLWindow::buttonName(DeviceID device_id, uint16_t raw_key) const
{
    if (device_maps.count(device_id)) {
        const auto& device = device_maps.at(device_id);
        switch (device.type) {
            case DeviceType::KEYBOARD: {
                const auto key = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(raw_key));
                const std::string keyname = SDL_GetKeyName(key);
                if (!keyname.empty()) // empty string on error
                    return keyname;
                break;
            }
            case DeviceType::GAMEPAD: {
                const char* name = SDL_GameControllerGetStringForButton(static_cast<SDL_GameControllerButton>(raw_key));
                if (name) // NULL on error
                    return name;
                break;
            }
            case DeviceType::LEGACY_JOYSTICK:
                return "Button " + std::to_string(raw_key);
        }
    }
    return "<unknown>";
}

void SDLWindow::setKeyBinding(DeviceID device_id, InputType inputevent, uint16_t raw_device_key)
{
    assert(device_maps.count(device_id));
    auto& device = device_maps.at(device_id);
    device.eventmap[inputevent] = {raw_device_key};
    device.buttonmap = toButtonMap(device.eventmap);
}

ButtonToEventsMap SDLWindow::knownButtonmapForDeviceName(const std::string& device_name)
{
    if (!known_mappings.count(device_name))
        return ButtonToEventsMap();
    return known_mappings.at(device_name).buttonmap;
}

std::vector<Event> SDLWindow::collectEvents()
{
    /* Note: because the SDL2 GameController API builds on top the SDL Joystick API,
     * game controllers can also receive joaystick events.
     * There are `if (joysticks.count(..))` checks at the switch, to avoid running
     * the input handling code for both APIs.
     */

    // as defined in the SDL documentation
    static const short int AXIS_MAX = 32767;
    static const short int AXIS_MIN = -32768;

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
                output.emplace_back(DeviceEvent(DeviceEventType::DISCONNECTED, sdl_event.cdevice.which));
            }
            break;
        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN: {
                assert(gamepads.count(sdl_event.cbutton.which));
                assert(device_maps.count(sdl_event.cbutton.which));
                const bool is_down = (sdl_event.type == SDL_CONTROLLERBUTTONDOWN);
                auto& buttonmap = device_maps.at(sdl_event.cbutton.which).buttonmap;
                for (const auto& input_event : buttonmap[sdl_event.cbutton.button])
                    output.emplace_back(InputEvent(input_event, is_down, sdl_event.cbutton.which));
                output.emplace_back(RawInputEvent(sdl_event.cdevice.which, sdl_event.cbutton.button, is_down));
            }
            break;
        case SDL_CONTROLLERAXISMOTION: {
                assert(gamepads.count(sdl_event.cbutton.which));
                assert(device_maps.count(sdl_event.cbutton.which));

                SDL_Event new_sdl_event;
                new_sdl_event.cbutton.timestamp = sdl_event.caxis.timestamp;
                new_sdl_event.cbutton.which = sdl_event.caxis.which;

                if (sdl_event.caxis.value == AXIS_MAX || sdl_event.caxis.value == AXIS_MIN) { // stick pushed
                    new_sdl_event.type = SDL_CONTROLLERBUTTONDOWN;
                    new_sdl_event.cbutton.state = SDL_PRESSED;
                }
                else { // stick released
                    new_sdl_event.type = SDL_CONTROLLERBUTTONUP;
                    new_sdl_event.cbutton.state = SDL_RELEASED;
                }

                switch (sdl_event.caxis.axis) {
                    case SDL_CONTROLLER_AXIS_LEFTX:
                    case SDL_CONTROLLER_AXIS_RIGHTX:
                        new_sdl_event.cbutton.button = (sdl_event.caxis.value < 0)
                            ? SDL_CONTROLLER_BUTTON_DPAD_LEFT
                            : SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
                        break;
                    case SDL_CONTROLLER_AXIS_LEFTY:
                    case SDL_CONTROLLER_AXIS_RIGHTY:
                        new_sdl_event.cbutton.button = (sdl_event.caxis.value < 0)
                            ? SDL_CONTROLLER_BUTTON_DPAD_UP
                            : SDL_CONTROLLER_BUTTON_DPAD_DOWN;
                        break;
                    default:
                        break;
                }

                SDL_PushEvent(&new_sdl_event);
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
                        auto& device = device_maps[iid];
                        auto& buttonmap = device.buttonmap;
                        if (buttonmap.empty())
                            buttonmap = knownButtonmapForDeviceName(name);
                        if (buttonmap.empty())
                            buttonmap = default_gamepad_mapping;
                        device.id = iid;
                        device.name = name;
                        device.type = DeviceType::GAMEPAD;
                        device.eventmap = toEventMap(buttonmap);
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
                        auto& device = device_maps[iid];
                        auto& buttonmap = device.buttonmap;
                        if (buttonmap.empty())
                            buttonmap = knownButtonmapForDeviceName(name);
                        if (buttonmap.empty())
                            buttonmap = default_joystick_mapping;
                        device.id = iid;
                        device.name = name;
                        device.type = DeviceType::LEGACY_JOYSTICK;
                        device.eventmap = toEventMap(buttonmap);
                        Log::info(LOG_INPUT_TAG) << "Joystick connected: " << name << "\n";
                    }
                }
            }
            output.emplace_back(DeviceEvent(DeviceEventType::CONNECTED, sdl_event.jdevice.which));
            break;
        case SDL_JOYDEVICEREMOVED:
            if (joysticks.count(sdl_event.jdevice.which)) {
                Log::info(LOG_INPUT_TAG) << "Joystick disconnected: "
                                         << SDL_JoystickName(joysticks.at(sdl_event.jdevice.which).get()) << "\n";
                joysticks.erase(sdl_event.jdevice.which);
                output.emplace_back(DeviceEvent(DeviceEventType::DISCONNECTED, sdl_event.jdevice.which));
            }
            break;
        case SDL_JOYHATMOTION:
            if (joysticks.count(sdl_event.jhat.which)) {
                assert(device_maps.count(sdl_event.jhat.which));
                auto& buttonmap = device_maps.at(sdl_event.jhat.which).buttonmap;
                uint16_t button = sdl_event.jhat.value;
                // turn off all hat keys - there can be only one direction active at a time
                static const auto all_hats = {SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT};
                for (const auto& hat : all_hats) {
                    for (const auto& event : buttonmap[hat])
                        output.emplace_back(InputEvent(event, false, sdl_event.jhat.which));

                    output.emplace_back(RawInputEvent(sdl_event.jhat.which, button, false));
                }
                // turn on only the current one
                for (const auto& event : buttonmap[button])
                    output.emplace_back(InputEvent(event, true, sdl_event.jhat.which));
                output.emplace_back(RawInputEvent(sdl_event.jhat.which, button, true));
            }
            break;
        case SDL_JOYAXISMOTION:
            if (joysticks.count(sdl_event.jhat.which)) {
                SDL_Event new_sdl_event;
                new_sdl_event.type = SDL_JOYHATMOTION;
                new_sdl_event.jhat.timestamp = sdl_event.jaxis.timestamp;
                new_sdl_event.jhat.which = sdl_event.jaxis.which;
                new_sdl_event.jhat.hat = 0;

                if (sdl_event.jaxis.value == AXIS_MAX || sdl_event.jaxis.value == AXIS_MIN) { // stick pushed
                    // assume 0, 2, 4, ... axis to be horizontal
                    // and 1, 3, 5, ... axis to be vertical
                    if (sdl_event.jaxis.axis % 2 == 0) {
                        new_sdl_event.jhat.value = (sdl_event.jaxis.value < 0)
                            ? SDL_HAT_LEFT
                            : SDL_HAT_RIGHT;
                    }
                    else {
                        new_sdl_event.jhat.value = (sdl_event.jaxis.value < 0)
                            ? SDL_HAT_UP
                            : SDL_HAT_DOWN;
                    }
                }
                else { // stick released
                    new_sdl_event.jhat.value = SDL_HAT_CENTERED;
                }

                SDL_PushEvent(&new_sdl_event);
            }
            break;
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
            if (joysticks.count(sdl_event.jbutton.which)) {
                assert(device_maps.count(sdl_event.jbutton.which));
                // reminder: buttons are stored on the upper byte
                const uint16_t button = (sdl_event.jbutton.button << 8) + 0xFF;
                const bool is_down = (sdl_event.type == SDL_JOYBUTTONDOWN);
                auto& buttonmap = device_maps.at(sdl_event.jbutton.which).buttonmap;
                for (const auto& input_event : buttonmap[button])
                    output.emplace_back(InputEvent(input_event, is_down, sdl_event.jbutton.which));
                output.emplace_back(RawInputEvent(sdl_event.jbutton.which, button, is_down));
            }
            break;
        case SDL_KEYUP:
            if (sdl_event.key.keysym.scancode == SDL_SCANCODE_F4 && SDL_GetModState() & KMOD_ALT)
                m_quit_requested = true;
            // do NOT break - the code below should run for KEYUP too
        case SDL_KEYDOWN:
            if (!sdl_event.key.repeat) {
                assert(device_maps.count(-1));
                const uint16_t scancode = sdl_event.key.keysym.scancode;
                const bool is_down = (sdl_event.type == SDL_KEYDOWN);
                auto& buttonmap = device_maps.at(-1).buttonmap;
                for (const auto& input_event : buttonmap[scancode])
                    output.emplace_back(InputEvent(input_event, is_down, -1));
                output.emplace_back(RawInputEvent(-1, scancode, is_down));
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
