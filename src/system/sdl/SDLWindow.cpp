#include "SDLWindow.h"

#include "SDLGraphicsContext.h"
#include "system/Log.h"
#include "system/Paths.h"


const std::string LOG_INPUT_TAG = "input";

SDLWindow::SDLWindow()
    : sdl(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER)
    , window("OpenBlok",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        960, 540,
        SDL_WINDOW_RESIZABLE)
    , gcx(window)
    , m_quit_requested(false)
{
    window.SetIcon(SDL2pp::Surface(Paths::data() + "icon.png"));
    SDL_GameControllerAddMappingsFromFile((Paths::data() + "gamecontrollerdb").c_str());
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
            input_mapping[scancode].emplace(elem.first);
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
            {
                // TODO: do not hardcode these values
                const bool isdown = sdl_event.type == SDL_CONTROLLERBUTTONDOWN;
                switch (sdl_event.cbutton.button) {
                    case SDL_CONTROLLER_BUTTON_DPAD_UP:
                        output.emplace_back(InputEvent(InputType::GAME_HARDDROP, isdown));
                        output.emplace_back(InputEvent(InputType::MENU_UP, isdown));
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                        output.emplace_back(InputEvent(InputType::GAME_SOFTDROP, isdown));
                        output.emplace_back(InputEvent(InputType::MENU_DOWN, isdown));
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                        output.emplace_back(InputEvent(InputType::GAME_MOVE_LEFT, isdown));
                        output.emplace_back(InputEvent(InputType::MENU_LEFT, isdown));
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                        output.emplace_back(InputEvent(InputType::GAME_MOVE_RIGHT, isdown));
                        output.emplace_back(InputEvent(InputType::MENU_RIGHT, isdown));
                        break;
                    case SDL_CONTROLLER_BUTTON_A:
                        output.emplace_back(InputEvent(InputType::GAME_ROTATE_LEFT, isdown));
                        output.emplace_back(InputEvent(InputType::MENU_OK, isdown));
                        break;
                    case SDL_CONTROLLER_BUTTON_B:
                        output.emplace_back(InputEvent(InputType::GAME_ROTATE_RIGHT, isdown));
                        output.emplace_back(InputEvent(InputType::MENU_CANCEL, isdown));
                        break;
                    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                        output.emplace_back(InputEvent(InputType::GAME_HOLD, isdown));
                        break;
                    case SDL_CONTROLLER_BUTTON_BACK:
                    case SDL_CONTROLLER_BUTTON_GUIDE:
                    case SDL_CONTROLLER_BUTTON_START:
                        output.emplace_back(InputEvent(InputType::GAME_PAUSE, isdown));
                        break;
                    default:
                        break;
                }
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
                output.emplace_back(InputEvent(InputType::GAME_MOVE_LEFT, false));
                output.emplace_back(InputEvent(InputType::GAME_MOVE_RIGHT, false));
                output.emplace_back(InputEvent(InputType::GAME_HARDDROP, false));
                output.emplace_back(InputEvent(InputType::GAME_SOFTDROP, false));
                switch (sdl_event.jhat.value) {
                    case SDL_HAT_LEFT:
                        output.emplace_back(InputEvent(InputType::GAME_MOVE_LEFT, true));
                        output.emplace_back(InputEvent(InputType::MENU_LEFT, true));
                        break;
                    case SDL_HAT_RIGHT:
                        output.emplace_back(InputEvent(InputType::GAME_MOVE_RIGHT, true));
                        output.emplace_back(InputEvent(InputType::MENU_RIGHT, true));
                        break;
                    case SDL_HAT_UP:
                        output.emplace_back(InputEvent(InputType::GAME_HARDDROP, true));
                        output.emplace_back(InputEvent(InputType::MENU_UP, true));
                        break;
                    case SDL_HAT_DOWN:
                        output.emplace_back(InputEvent(InputType::GAME_SOFTDROP, true));
                        output.emplace_back(InputEvent(InputType::MENU_DOWN, true));
                        break;
                    default:
                        break;
                }
            }
            break;
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
            if (joysticks.count(sdl_event.jhat.which)) {
                // TODO: do not hardcode these values
                const auto button = sdl_event.jbutton.button;
                const bool isdown = sdl_event.type == SDL_JOYBUTTONDOWN;
                switch (button) {
                    case 0:
                    case 1:
                        output.emplace_back(InputEvent(InputType::GAME_ROTATE_LEFT, isdown));
                        break;
                    case 2:
                    case 3:
                        output.emplace_back(InputEvent(InputType::GAME_ROTATE_RIGHT, isdown));
                        break;
                    case 4:
                        output.emplace_back(InputEvent(InputType::GAME_HOLD, isdown));
                        break;
                    case 8:
                    case 9:
                        output.emplace_back(InputEvent(InputType::GAME_PAUSE, isdown));
                        break;
                    default:
                        break;
                }
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
                if (input_mapping.count(scancode)) {
                    for (const auto& input_event : input_mapping.at(scancode))
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
