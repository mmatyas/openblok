#include "game/AppContext.h"
#include "system/Log.h"
#include "game/InitState.h"

#include <chrono>
#include <memory>
#include <thread>
#include <assert.h>


int main(int argc, char const *argv[])
{
    Log::info("main") << "OpenBlok v0.0.0 by Mátyás Mustoha\n";
    AppContext app;
    if (!app.init())
        return 1;


    app.states.emplace(std::make_unique<InitState>());


    constexpr auto frame_duration = std::chrono::duration<int, std::ratio<1, 60>>(1);
    auto frame_starttime = std::chrono::steady_clock::now();
    auto frame_endtime = frame_starttime + frame_duration;

    while (!app.events->quit_requested()) {
        auto events = app.events->collect();

        assert(app.states.size());
        app.states.top()->update(events, app);
        app.states.top()->draw();

        app.gcx->render();

        // frame rate limiting
        std::this_thread::sleep_until(frame_endtime);
        frame_starttime = std::chrono::steady_clock::now();
        frame_endtime = frame_starttime + frame_duration;
    }

    return 0;
}
