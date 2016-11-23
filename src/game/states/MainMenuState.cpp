#include "MainMenuState.h"

#include "game/AppContext.h"
#include "substates/mainmenu/Base.h"


MainMenuState::MainMenuState(AppContext& app)
{
    states.emplace_back(std::make_unique<SubStates::MainMenu::Base>(*this, app));
}

MainMenuState::~MainMenuState() = default;

void MainMenuState::update(const std::vector<Event>& events, AppContext& app)
{
    for (const auto& event : events) {
        if (event.type == EventType::WINDOW && event.window == WindowEvent::RESIZED) {
            for (auto& state : states)
                state->updatePositions(app.gcx());
        }
    }
    states.back()->update(*this, events, app);
}

void MainMenuState::draw(GraphicsContext& gcx)
{
    states.back()->draw(*this, gcx);
}
