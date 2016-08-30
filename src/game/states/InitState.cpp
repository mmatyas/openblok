#include "InitState.h"

#include "game/AppContext.h"
#include "game/Resources.h"
#include "game/states/MainMenuState.h"
#include "system/Localize.h"
#include "system/Log.h"

#include <cassert>
#include <chrono>


InitState::InitState(AppContext& app)
{
    app.gcx->loadFont(ResourceID::FONT_REGULAR, "data/regular.otf", 30);
    app.gcx->cacheText(ResourceID::TEX_LOADING, tr("LOADING..."),
                       ResourceID::FONT_REGULAR, 0xFFFFFF_rgb);

    worker = std::async(std::launch::async, [&app](){
        Log::info("init") << "Worker launched\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Log::info("init") << "Worker finished\n";
    });
    assert(worker.valid());
}

void InitState::update(const std::vector<InputEvent>&, AppContext& app)
{
    auto worker_status = worker.wait_for(std::chrono::milliseconds(0));
    if (worker_status == std::future_status::ready) {
        try {
            worker.get();
            app.states.emplace(std::make_unique<MainMenuState>(app));
        }
        catch (const std::runtime_error& err) {
            Log::error("init") << err.what();
            app.states.pop();
        }
    }
}

void InitState::draw(GraphicsContext& gcx)
{
    gcx.drawTexture(ResourceID::TEX_LOADING, 10, 10);
}
