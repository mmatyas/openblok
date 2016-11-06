#include "AppContext.h"

#include "system/Log.h"

#ifndef OPENBLOK_DATADIR
#define OPENBLOK_DATADIR "data"
#endif
const std::string DATADIR = std::string(OPENBLOK_DATADIR) + "/";


bool AppContext::init()
{
    const std::string log_tag = "init";
    try {
        Log::info(log_tag) << "Initializing video...\n";
        m_window = Window::create();

        srand(time(nullptr));
    }
    catch (const std::exception& err) {
        Window::showErrorMessage(err.what());
        return false;
    }

    return true;
}
