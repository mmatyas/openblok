#include "MainMenuState.h"

#include "game/AppContext.h"
#include "substates/mainmenu/Base.h"
#include "system/AudioContext.h"
#include "system/Paths.h"
#include "system/Texture.h"


MainMenuState::MainMenuState(AppContext& app)
    : padconnect_phase1(std::chrono::seconds(3), [](double){}, [this](){ padconnect_phase2.restart(); })
    , padconnect_phase2(std::chrono::seconds(3), [](double t){ return (1.0 - t) * 0xFF; })
    , tex_padconnected(app.gcx().loadTexture(Paths::data() + "gamepad-connect.png"))
{
    padconnect_phase1.stop();
    padconnect_phase2.update(padconnect_phase2.length()); // set alpha to 0
    states.emplace_back(std::make_unique<SubStates::MainMenu::Base>(*this, app));
}

MainMenuState::~MainMenuState() = default;

void MainMenuState::update(const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        switch (event.type) {
            case EventType::WINDOW:
                switch (event.window) {
                    case WindowEvent::RESIZED:
                        for (auto& state : states)
                            state->updatePositions(app.gcx());
                        break;
                    case WindowEvent::FOCUS_LOST:
                        app.audio().pauseAll();
                        break;
                    case WindowEvent::FOCUS_GAINED:
                        app.audio().resumeAll();
                        break;
                    default:
                        break;
                }
                break;
            case EventType::DEVICE:
                switch (event.device.type) {
                    case DeviceEventType::CONNECTED:
                        padconnect_phase1.restart();
                        padconnect_phase2.restart();
                        padconnect_phase2.pause();
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    padconnect_phase1.update(Timing::frame_duration);
    padconnect_phase2.update(Timing::frame_duration);
    tex_padconnected->setAlpha(padconnect_phase2.value());

    // this may pop the main menu state
    states.back()->update(*this, events, app);
}

void MainMenuState::draw(GraphicsContext& gcx)
{
    states.back()->draw(*this, gcx);
    tex_padconnected->drawAt(gcx.screenWidth() - 30 - tex_padconnected->width(),
                             gcx.screenHeight() - 30 - tex_padconnected->height());
}
