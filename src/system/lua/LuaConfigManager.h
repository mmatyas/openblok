#pragma once

#include "system/ConfigManager.h"


class LuaConfigManager : public ConfigManager {
public:
    InputScancodeMap loadInputMapping(const std::string& scriptfile) final;
    void saveInputMapping(const InputScancodeMap&, const std::string& scriptfile) final;

private:
};
