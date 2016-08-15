#include "system/EventCollector.h"
#include "system/GraphicsContext.h"
#include "system/Log.h"

#include <chrono>
#include <memory>
#include <thread>


class App {
public:
    bool init();

    std::unique_ptr<GraphicsContext> gcx;
    std::unique_ptr<EventCollector> events;
};

bool App::init()
{
    const std::string log_tag = "init";
    try {
        Log::info(log_tag) << "Initializing video...\n";
        gcx = GraphicsContext::create();

        Log::info(log_tag) << "Initializing event system...\n";
        events = EventCollector::create();
    }
    catch (const std::exception& err) {
        Log::error(log_tag) << err.what() << std::endl;
        return false;
    }

    return true;
}


int main(int argc, char const *argv[])
{
    Log::info("main") << "OpenBlok v0.0.0 by Mátyás Mustoha\n";
    App app;
    if (!app.init())
        return 1;


    constexpr auto frame_duration = std::chrono::duration<int, std::ratio<1, 60>>(1);
    auto frame_starttime = std::chrono::steady_clock::now();
    auto frame_endtime = frame_starttime + frame_duration;

    while (!app.events->quit_requested()) {
        auto events = app.events->collect();

        app.gcx->render();

        // frame rate limiting
        std::this_thread::sleep_until(frame_endtime);
        frame_starttime = std::chrono::steady_clock::now();
        frame_endtime = frame_starttime + frame_duration;
    }

    return 0;
}
