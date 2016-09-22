#include "MainMenuState.h"

#include "SinglePlayState.h"
#include "game/AppContext.h"
#include "game/components/GameplayResources.h"


MainMenuState::MainMenuState(AppContext&)
{
}

void MainMenuState::update(const std::vector<InputEvent>&, AppContext& app)
{
    GameplayResources::Textures::loadAll(*app.gcx);
    app.states.emplace(std::make_unique<SinglePlayState>(app));
}

void MainMenuState::draw(GraphicsContext&)
{
}
