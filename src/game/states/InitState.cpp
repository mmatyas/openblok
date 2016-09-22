#include "InitState.h"

#include "game/AppContext.h"
#include "game/CommonResources.h"
#include "game/states/MainMenuState.h"
#include "system/Localize.h"
#include "system/Log.h"

#include <cassert>
#include <chrono>


InitState::InitState(AppContext& app)
{
    using Fonts = CommonResources::Fonts;
    Fonts::HEADER = app.gcx->loadFont(app.fonts->find("Bebas Neue"), 30);
    tex_loading = app.gcx->renderText(tr("LOADING..."),
                                      Fonts::HEADER, 0xFFFFFF_rgb);

    worker = std::async(std::launch::async, [&app](){
        //std::this_thread::sleep_for(std::chrono::seconds(1));
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
    gcx.drawTexture(tex_loading, 10, 10);
}
