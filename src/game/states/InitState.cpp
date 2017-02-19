#include "InitState.h"

#include "game/AppContext.h"
#include "game/GameConfigFile.h"
#include "game/Theme.h"
#include "game/states/MainMenuState.h"
#include "system/AudioContext.h"
#include "system/Log.h"
#include "system/Paths.h"
#include "system/Window.h"

#include <assert.h>


InitState::InitState(AppContext& app)
{
    const auto mappings = app.inputconfig().load(Paths::config() + "input.cfg");
    app.inputconfig().save(mappings, Paths::config() + "input.cfg");
    app.window().setInputConfig(mappings);

    const auto config = GameConfigFile::load(Paths::config() + "game.cfg");
    app.sysconfig() = std::get<0>(config);
    app.wellconfig() = std::get<1>(config);
    if (app.sysconfig().fullscreen)
        app.window().toggleFullscreen();
    if (!app.sysconfig().sfx)
        app.audio().toggleSFXMute();
    if (!app.sysconfig().music)
        app.audio().toggleMusicMute();

    Log::info("init") << "Loading resources from '" << Paths::data() << "'\n";

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
