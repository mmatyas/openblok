#include "SDLWindow.h"

#include "SDLGraphicsContext.h"
#include "system/Log.h"


const std::string LOG_INPUT_TAG = "input";

SDLWindow::SDLWindow()
    : sdl(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)
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

void SDLWindow::setInputMapping(std::map<InputType, std::vector<uint16_t>> mapping)
{
    for (const auto& elem : mapping) {
        for (const auto& scancode : elem.second)
            input_mapping[scancode].emplace(elem.first);
    }
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
        case SDL_JOYDEVICEADDED:
            {
                auto joy = std::unique_ptr<SDL_Joystick, std::function<void(SDL_Joystick*)>>(
                    SDL_JoystickOpen(sdl_event.jdevice.which),
                    [](SDL_Joystick* ptr){
                        if (SDL_JoystickGetAttached(ptr)) SDL_JoystickClose(ptr); });

                if (joy) {
                    auto iid = SDL_JoystickInstanceID(joy.get()); // returns negative on error
                    if (iid >= 0) {
                        joysticks[iid] = std::move(joy);
                        Log::info(LOG_INPUT_TAG) << "Controller connected: "
                                                 << SDL_JoystickName(joysticks[iid].get()) << "\n";
                    }
                }
            }
            break;
        case SDL_JOYDEVICEREMOVED:
            if (joysticks.count(sdl_event.jdevice.which)) {
                Log::info(LOG_INPUT_TAG) << "Controller disconnected: "
                                         << SDL_JoystickName(joysticks[sdl_event.jdevice.which].get()) << "\n";
                joysticks.erase(sdl_event.jdevice.which);
            }
            break;
        case SDL_JOYHATMOTION:
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
            break;
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
            {
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
