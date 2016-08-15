#include "system/GraphicsContext.h"

#include "system/Log.h"

#include <memory>


class App {
public:
    bool init();

    std::unique_ptr<GraphicsContext> gcx;
};

bool App::init()
{
    const std::string log_tag = "init";
    try {
        Log::info(log_tag) << "Initializing video...\n";
        gcx = GraphicsContext::create();
        Log::info(log_tag) << "Initializing video... ok\n";
    }
    catch(const std::exception& err) {
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

    return 0;
}
