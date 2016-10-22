#include "ScriptLoader.h"

#include "ClassicRotationFn.h"
#include "lua/LuaRotationFn.h"
#include "system/Log.h"

#include <exception>


const std::string LOG_TAG = "plugin";

std::unique_ptr<RotationFn> ScriptLoader::loadRotationFn(const std::string& scriptfile)
{
    std::unique_ptr<RotationFn> plugin;
    try {
        plugin = std::make_unique<LuaRotationFn>(scriptfile);
    }
    catch (const std::runtime_error* err) {
        Log::warning(LOG_TAG) << err->what() << "\n";
        plugin = std::make_unique<ClassicRotationFn>();
    }

    Log::info(LOG_TAG) << plugin->pluginName() << " loaded\n";
    return plugin;
}
