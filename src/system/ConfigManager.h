#pragma once

#include "InputMap.h"


class ConfigManager {
public:
    Devices loadInputMappings(const std::string& path);
    void saveInputMapping(const Devices&, const std::string& path);
};
