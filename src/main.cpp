#include "game/AppContext.h"
#include "game/GameState.h"
#include "game/Timing.h"
#include "game/states/InitState.h"
#include "system/Log.h"

#include <chrono>
#include <memory>
#include <thread>
#include <assert.h>


int main(int, const char**)
{
    Log::info("main") << "OpenBlok v0.1.0 by Mátyás Mustoha\n";
    AppContext app;
    if (!app.init())
        return 1;


    try { app.states().emplace(std::make_unique<InitState>(app)); }
    catch (const std::exception& err) {
        app.window().showErrorMessage(err.what());
        return 1;
    }


    auto frame_starttime = std::chrono::steady_clock::now();
    auto frame_endtime = frame_starttime + Timing::frame_duration;

    while (!app.window().quitRequested()) {
        try {
            auto events = app.window().collectEvents();

            assert(app.states().size());
            app.states().top()->update(events, app);
            if (app.states().empty())
                break;

            app.states().top()->draw(app.gcx());
            app.gcx().render();
        }
        catch (const std::exception& err) {
            app.window().showErrorMessage(err.what());
            return 1;
        }

        // frame rate limiting
        std::this_thread::sleep_until(frame_endtime);
        frame_starttime = std::chrono::steady_clock::now();
        frame_endtime = frame_starttime + Timing::frame_duration;
    }

    return 0;
}
