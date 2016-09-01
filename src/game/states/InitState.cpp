#include "InitState.h"

#include "game/AppContext.h"
#include "game/MinoFactory.h"
#include "game/Resources.h"
#include "game/states/MainMenuState.h"
#include "system/Localize.h"
#include "system/Log.h"

#include <cassert>
#include <chrono>


InitState::InitState(AppContext& app)
{
    app.gcx->loadFont(FontID::REGULAR, "data/regular.otf", 30);
    app.gcx->cacheText(TexID::LOADING, tr("LOADING..."),
                       FontID::REGULAR, 0xFFFFFF_rgb);


    app.gcx->loadTexture(TexID::MINO_I, "data/mino.png", MinoFactory::color(Mino::Type::I));
    app.gcx->loadTexture(TexID::MINO_J, "data/mino.png", MinoFactory::color(Mino::Type::J));
    app.gcx->loadTexture(TexID::MINO_L, "data/mino.png", MinoFactory::color(Mino::Type::L));
    app.gcx->loadTexture(TexID::MINO_O, "data/mino.png", MinoFactory::color(Mino::Type::O));
    app.gcx->loadTexture(TexID::MINO_S, "data/mino.png", MinoFactory::color(Mino::Type::S));
    app.gcx->loadTexture(TexID::MINO_T, "data/mino.png", MinoFactory::color(Mino::Type::T));
    app.gcx->loadTexture(TexID::MINO_Z, "data/mino.png", MinoFactory::color(Mino::Type::Z));

    app.gcx->loadTexture(TexID::MATRIXBG, "data/matrix_pattern.png");


    worker = std::async(std::launch::async, [&app](){
        Log::info("init") << "Worker launched\n";
        //std::this_thread::sleep_for(std::chrono::seconds(1));
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
    gcx.drawTexture(TexID::LOADING, 10, 10);
}
