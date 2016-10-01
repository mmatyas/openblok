#include "game/AppContext.h"
#include "game/GameState.h"
#include "game/states/InitState.h"
#include "system/Log.h"

#include <chrono>
#include <memory>
#include <thread>
#include <assert.h>


int main(int, const char**)
{
    Log::info("main") << "OpenBlok v0.0.0 by Mátyás Mustoha\n";
    AppContext app;
    if (!app.init())
        return 1;


    app.states().emplace(std::make_unique<InitState>(app));


    auto frame_starttime = std::chrono::steady_clock::now();
    auto frame_endtime = frame_starttime + GameState::frame_duration;

    while (!app.window().quitRequested()) {
        auto events = app.window().collectEvents();

        assert(app.states().size());
        app.states().top()->update(events, app);
        if (app.states().empty())
            break;

        app.states().top()->draw(app.gcx());
        app.gcx().render();

        // frame rate limiting
        std::this_thread::sleep_until(frame_endtime);
        frame_starttime = std::chrono::steady_clock::now();
        frame_endtime = frame_starttime + GameState::frame_duration;
    }

    return 0;
}
