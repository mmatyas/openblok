#include "ScriptLoader.h"

#include "ClassicRotationFn.h"
#include "lua/LuaRotationFn.h"
#include "system/Log.h"

#include <exception>


const std::string LOG_TAG = "plugin";

std::unique_ptr<RotationFn> ScriptLoader::loadRotationFn(const std::string& scriptfile)
{
    try {
        Log::info(LOG_TAG) << "Loading rotation plugin: " << scriptfile << "\n";
        return std::make_unique<LuaRotationFn>(scriptfile);
    }
    catch (const std::runtime_error* err) {
        Log::warning(LOG_TAG) << err->what() << "\n";
    }

    return std::make_unique<ClassicRotationFn>();
}
