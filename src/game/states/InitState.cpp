#include "InitState.h"

#include "game/AppContext.h"
#include "game/components/MinoStorage.h"
#include "game/states/MainMenuState.h"
#include "system/Log.h"


InitState::InitState(AppContext& app)
{
    auto mapping = app.config().loadInputMapping("data/config/input.cfg");
    app.config().saveInputMapping(mapping, "data/config/input.cfg");
    app.window().setInputMapping(mapping);

    Log::info("init") << "Loading resources\n";

    MinoStorage::loadTintedMinos(app.gcx(), "data/mino.png");
    MinoStorage::loadTintedGhosts(app.gcx(), "data/ghost.png");
    MinoStorage::loadMatrixCell(app.gcx(), "data/matrix.png");

    app.states().emplace(std::make_unique<MainMenuState>(app));
}

void InitState::update(const std::vector<Event>&, AppContext& app)
{
    app.states().pop();
}

void InitState::draw(GraphicsContext&)
{
    assert(false);
}
