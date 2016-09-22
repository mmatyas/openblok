#include "InitState.h"

#include "game/AppContext.h"
#include "game/CommonResources.h"
#include "game/components/MinoFactory.h"
#include "game/states/MainMenuState.h"
#include "system/Localize.h"
#include "system/Log.h"

#include <cassert>
#include <chrono>


InitState::InitState(AppContext& app)
{
    CommonFonts::HEADER = app.gcx->loadFont(app.fonts->find("Bebas Neue"), 30);
    CommonTextures::LOADING = app.gcx->renderText(tr("LOADING..."),
                                                  CommonFonts::HEADER, 0xFFFFFF_rgb);


    CommonTextures::MINO_I = app.gcx->loadTexture("data/mino.png", MinoFactory::color(Piece::Type::I));
    CommonTextures::MINO_J = app.gcx->loadTexture("data/mino.png", MinoFactory::color(Piece::Type::J));
    CommonTextures::MINO_L = app.gcx->loadTexture("data/mino.png", MinoFactory::color(Piece::Type::L));
    CommonTextures::MINO_O = app.gcx->loadTexture("data/mino.png", MinoFactory::color(Piece::Type::O));
    CommonTextures::MINO_S = app.gcx->loadTexture("data/mino.png", MinoFactory::color(Piece::Type::S));
    CommonTextures::MINO_T = app.gcx->loadTexture("data/mino.png", MinoFactory::color(Piece::Type::T));
    CommonTextures::MINO_Z = app.gcx->loadTexture("data/mino.png", MinoFactory::color(Piece::Type::Z));
    CommonTextures::MINO_GHOST = app.gcx->loadTexture("data/ghost.png");

    CommonTextures::MATRIXBG = app.gcx->loadTexture("data/matrix_pattern.png");
    CommonTextures::GAMEPLAYBG = app.gcx->loadTexture("data/gamebg_pattern.png");


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
    gcx.drawTexture(CommonTextures::LOADING, 10, 10);
}
