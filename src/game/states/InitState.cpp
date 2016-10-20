#include "InitState.h"

#include "game/AppContext.h"
#include "game/components/MinoStorage.h"
#include "game/states/MainMenuState.h"
#include "system/Log.h"


InitState::InitState(AppContext& app)
{
    Log::info("init") << "Loading resources\n";

    MinoStorage::loadTintedMinos(app.gcx(), "data/mino.png");
    MinoStorage::loadTintedGhosts(app.gcx(), "data/ghost.png");
    MinoStorage::loadMatrixCell(app.gcx(), "data/matrix.png");
}

void InitState::update(const std::vector<InputEvent>&, AppContext& app)
{
    app.states().emplace(std::make_unique<MainMenuState>(app));
}

void InitState::draw(GraphicsContext&)
{
}
