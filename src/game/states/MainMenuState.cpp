#include "MainMenuState.h"

#include "game/AppContext.h"
#include "game/CommonResources.h"
#include "SinglePlayState.h"


MainMenuState::MainMenuState(AppContext&)
{
}

void MainMenuState::update(const std::vector<InputEvent>&, AppContext& app)
{
    app.states.emplace(std::make_unique<SinglePlayState>(app));
}

void MainMenuState::draw(GraphicsContext&)
{
}
