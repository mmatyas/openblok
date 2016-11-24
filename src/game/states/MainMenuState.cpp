#include "MainMenuState.h"

#include "game/AppContext.h"
#include "substates/mainmenu/Base.h"
#include "system/AudioContext.h"


MainMenuState::MainMenuState(AppContext& app)
{
    states.emplace_back(std::make_unique<SubStates::MainMenu::Base>(*this, app));
}

MainMenuState::~MainMenuState() = default;

void MainMenuState::update(const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        if (event.type == EventType::WINDOW) {
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
        }
    }
    states.back()->update(*this, events, app);
}

void MainMenuState::draw(GraphicsContext& gcx)
{
    states.back()->draw(*this, gcx);
}
