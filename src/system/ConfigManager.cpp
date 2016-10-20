#include "ConfigManager.h"

#include "lua/LuaConfigManager.h"


std::unique_ptr<ConfigManager> ConfigManager::create()
{
    return std::make_unique<LuaConfigManager>();
}
