#include "MainMenuState.h"

#include "SinglePlayState.h"
#include "game/AppContext.h"
#include "game/components/MinoStorage.h"


MainMenuState::MainMenuState(AppContext& app)
{
    MinoStorage::loadTintedMinos(app.gcx(), "data/mino.png");
    MinoStorage::loadTintedGhosts(app.gcx(), "data/ghost.png");
    MinoStorage::loadMatrixCell(app.gcx(), "data/matrix.png");
}

void MainMenuState::update(const std::vector<InputEvent>&, AppContext& app)
{
    app.states().emplace(std::make_unique<SinglePlayState>(app));
}

void MainMenuState::draw(GraphicsContext&)
{
}
