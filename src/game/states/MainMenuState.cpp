#include "MainMenuState.h"

#include "substates/mainmenu/Base.h"


MainMenuState::MainMenuState(AppContext& app)
{
    states.emplace_back(std::make_unique<SubStates::MainMenu::Base>(*this, app));
}

MainMenuState::~MainMenuState() = default;

void MainMenuState::update(const std::vector<Event>& events, AppContext& app)
{
    states.back()->update(*this, events, app);
}

void MainMenuState::draw(GraphicsContext& gcx)
{
    states.back()->draw(*this, gcx);
}
