#include "AppContext.h"

#include "system/Log.h"


bool AppContext::init()
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
