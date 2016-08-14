#include "system/GraphicsContext.h"

#include <iostream>
#include <memory>


class App {
public:
    App();

    std::unique_ptr<GraphicsContext> gcx;
};

App::App()
{
    std::cout << "[info] Initializing video...\n";
    gcx = GraphicsContext::create();
    std::cout << "[info] Initializing video... ok\n";
}


int main(int argc, char const *argv[])
{
    App app;
    return 0;
}
