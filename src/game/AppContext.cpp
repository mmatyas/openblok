#include "AppContext.h"

#include "system/Log.h"

#include <cstdlib>
#include <ctime>


bool AppContext::init()
{
    const std::string log_tag = "init";
    try {
        Log::info(log_tag) << "Initializing video...\n";
        m_window = Window::create();

        std::srand(std::time(nullptr));
    }
    catch (const std::exception& err) {
        Window::showErrorMessage(err.what());
        return false;
    }

    return true;
}
