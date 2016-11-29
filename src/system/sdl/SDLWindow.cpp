#include "SDLWindow.h"

#include "version.h"
#include "SDLGraphicsContext.h"
#include "system/Log.h"
#include "system/Paths.h"


const std::string LOG_INPUT_TAG = "input";

SDLWindow::SDLWindow()
    : sdl(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER)
    , window(std::string("OpenBlok ") + game_version,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        960, 540,
        SDL_WINDOW_RESIZABLE)
    , gcx(window)
    , m_quit_requested(false)
{
    window.SetIcon(SDL2pp::Surface(Paths::data() + "icon.png"));
    SDL_GameControllerAddMappingsFromFile((Paths::data() + "gamecontrollerdb").c_str());

    // TODO: add per-device setting and loading from file
    gamepad_mapping = {
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
    };

    // In SDL, the legacy joystick API uses 1 byte for button indices,
    // and also 1 byte for hats, both starting from 0. Here we store
    // them in the same map using 2 bytes, buttons in the upper byte
    // with 0xFF below, and hats in the lower byte.
    joystick_mapping = {
        {SDL_HAT_UP, {InputType::GAME_HARDDROP, InputType::MENU_UP}},
        {SDL_HAT_DOWN, {InputType::GAME_SOFTDROP, InputType::MENU_DOWN}},
        {SDL_HAT_LEFT, {InputType::GAME_MOVE_LEFT, InputType::MENU_LEFT}},
        {SDL_HAT_RIGHT, {InputType::GAME_MOVE_RIGHT, InputType::MENU_RIGHT}},
        {0 + 0xFF, {InputType::GAME_ROTATE_LEFT}},
        {(1 << 8) + 0xFF, {InputType::GAME_ROTATE_LEFT}},
        {(2 << 8) + 0xFF, {InputType::GAME_ROTATE_RIGHT}},
        {(3 << 8) + 0xFF, {InputType::GAME_ROTATE_RIGHT}},
        {(4 << 8) + 0xFF, {InputType::GAME_HOLD}},
        {(5 << 8) + 0xFF, {InputType::GAME_HOLD}},
        {(8 << 8) + 0xFF, {InputType::GAME_PAUSE}},
        {(9 << 8) + 0xFF, {InputType::GAME_PAUSE}},
    };
}

void SDLWindow::toggleFullscreen()
{
    window.SetFullscreen(window.GetFlags() ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void SDLWindow::requestScreenshot(const std::string& path)
{
    gcx.requestScreenshot(window, path);
}

void SDLWindow::setInputMapping(std::map<InputType, std::vector<uint16_t>> mapping)
{
    for (const auto& elem : mapping) {
        for (const auto& scancode : elem.second)
            keyboard_mapping[scancode].emplace(elem.first);
    }
}

std::vector<Event> SDLWindow::collectEvents()
{
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
            {
                auto gamepad = std::unique_ptr<SDL_GameController, std::function<void(SDL_GameController*)>>(
                    SDL_GameControllerOpen(sdl_event.cdevice.which),
                    [](SDL_GameController* ptr){
                        if (SDL_GameControllerGetAttached(ptr)) SDL_GameControllerClose(ptr); });

                if (gamepad) {
                    auto joystick = SDL_GameControllerGetJoystick(gamepad.get());
                    auto iid = SDL_JoystickInstanceID(joystick); // returns negative on error
                    if (iid >= 0) {
                        gamepads[iid] = std::move(gamepad);
                        Log::info(LOG_INPUT_TAG) << "Gamepad connected: "
                                                 << SDL_GameControllerName(gamepads[iid].get()) << "\n";
                    }
                }
            }
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            if (gamepads.count(sdl_event.cdevice.which)) {
                Log::info(LOG_INPUT_TAG) << "Gamepad disconnected: "
                                         << SDL_GameControllerName(gamepads[sdl_event.cdevice.which].get()) << "\n";
                gamepads.erase(sdl_event.cdevice.which);
            }
            break;
        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN:
            if (gamepad_mapping.count(sdl_event.cbutton.button)) {
                for (const auto& input_event : gamepad_mapping.at(sdl_event.cbutton.button))
                    output.emplace_back(InputEvent(input_event, sdl_event.type == SDL_CONTROLLERBUTTONDOWN));
            }
            break;
        case SDL_JOYDEVICEADDED:
            if (!SDL_IsGameController(sdl_event.jdevice.which)) {
                auto joy = std::unique_ptr<SDL_Joystick, std::function<void(SDL_Joystick*)>>(
                    SDL_JoystickOpen(sdl_event.jdevice.which),
                    [](SDL_Joystick* ptr){
                        if (SDL_JoystickGetAttached(ptr)) SDL_JoystickClose(ptr); });

                if (joy) {
                    auto iid = SDL_JoystickInstanceID(joy.get()); // returns negative on error
                    if (iid >= 0) {
                        joysticks[iid] = std::move(joy);
                        Log::info(LOG_INPUT_TAG) << "Joystick connected: "
                                                 << SDL_JoystickName(joysticks[iid].get()) << "\n";
                    }
                }
            }
            break;
        case SDL_JOYDEVICEREMOVED:
            if (joysticks.count(sdl_event.jdevice.which)) {
                Log::info(LOG_INPUT_TAG) << "Joystick disconnected: "
                                         << SDL_JoystickName(joysticks[sdl_event.jdevice.which].get()) << "\n";
                joysticks.erase(sdl_event.jdevice.which);
            }
            break;
        case SDL_JOYHATMOTION:
            if (joysticks.count(sdl_event.jhat.which)) {
                if (joysticks.count(sdl_event.jhat.which)) {
                    uint16_t button = sdl_event.jhat.value;
                    // turn off all hat keys - there can be only one direction active at a time
                    for (const auto& hat : {SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT}) {
                        for (const auto& event : joystick_mapping.at(hat))
                            output.emplace_back(InputEvent(event, false));
                    }
                    // turn on only the current one
                    for (const auto& event : joystick_mapping.at(button))
                        output.emplace_back(InputEvent(event, true));
                }
            }
            break;
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
            if (joysticks.count(sdl_event.jbutton.which)) {
                // reminder: buttons are stored on the upper byte
                uint16_t button = (sdl_event.jbutton.button << 8) + 0xFF;
                for (const auto& event : joystick_mapping.at(button))
                    output.emplace_back(InputEvent(event, false));
            }
            break;
        case SDL_KEYUP:
            if (sdl_event.key.keysym.scancode == SDL_SCANCODE_F4 && SDL_GetModState() & KMOD_ALT)
                m_quit_requested = true;
            // do NOT break - the code below should run for KEYUP too
        case SDL_KEYDOWN:
            if (!sdl_event.key.repeat) {
                uint16_t scancode = sdl_event.key.keysym.scancode;
                if (keyboard_mapping.count(scancode)) {
                    for (const auto& input_event : keyboard_mapping.at(scancode))
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
