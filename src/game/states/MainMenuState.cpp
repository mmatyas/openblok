#include "MainMenuState.h"

#include "SinglePlayState.h"
#include "game/AppContext.h"
#include "game/components/PieceFactory.h"
#include "game/components/rotations/SRS.h"


MainMenuState::MainMenuState(AppContext&)
{
    PieceFactory::changeInitialPositions(Rotations::SRS().initialPositions());
}

void MainMenuState::update(const std::vector<InputEvent>&, AppContext& app)
{
    app.states().emplace(std::make_unique<SinglePlayState>(app));
}

void MainMenuState::draw(GraphicsContext&)
{
}
