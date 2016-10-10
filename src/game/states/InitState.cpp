#include "InitState.h"

#include "game/AppContext.h"
#include "game/states/MainMenuState.h"


InitState::InitState(AppContext&)
{
}

void InitState::update(const std::vector<InputEvent>&, AppContext& app)
{
    app.states().emplace(std::make_unique<MainMenuState>(app));
}

void InitState::draw(GraphicsContext&)
{
}
