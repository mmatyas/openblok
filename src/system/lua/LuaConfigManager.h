#pragma once

#include "system/ConfigManager.h"


class LuaConfigManager : public ConfigManager {
public:
    ScancodeMap loadInputMapping(const std::string& scriptfile) final;
    void saveInputMapping(const ScancodeMap&, const std::string& scriptfile) final;
};
