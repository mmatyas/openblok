#include "system/GraphicsContext.h"

#include <iostream>
#include <memory>


class App {
public:
    bool init();

    std::unique_ptr<GraphicsContext> gcx;
};

bool App::init()
{
    try {
        std::cout << "[info] Initializing video...\n";
        gcx = GraphicsContext::create();
        std::cout << "[info] Initializing video... ok\n";
    }
    catch(const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return false;
    }

    return true;
}


int main(int argc, char const *argv[])
{
    App app;
    if (!app.init())
        return 1;

    return 0;
}
