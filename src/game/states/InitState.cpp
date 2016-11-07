#include "InitState.h"

#include "game/AppContext.h"
#include "game/components/MinoStorage.h"
#include "game/states/MainMenuState.h"
#include "system/Log.h"
#include "system/Paths.h"


InitState::InitState(AppContext& app)
{
    auto mapping = app.config().loadInputMapping(Paths::config() + "input.cfg");
    app.config().saveInputMapping(mapping, Paths::config() + "input.cfg");
    app.window().setInputMapping(mapping);

    Log::info("init") << "Loading resources from '" << Paths::data() << "'\n";

    MinoStorage::loadTintedMinos(app.gcx(), Paths::data() + "mino.png");
    MinoStorage::loadTintedGhosts(app.gcx(), Paths::data() + "ghost.png");
    MinoStorage::loadMatrixCell(app.gcx(), Paths::data() + "matrix.png");

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
