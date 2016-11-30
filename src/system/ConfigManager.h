#pragma once

#include "InputMap.h"


class ConfigManager {
public:
    DeviceMaps loadInputMappings(const std::string& path);
    void saveInputMapping(const DeviceMaps&, const std::string& path);
};
