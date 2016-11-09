#pragma once

#include "Event.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdint.h>


class ConfigManager {
public:
    using DeviceID = std::string;
    using ButtonMap = std::map<InputType, std::vector<uint16_t>>;
    using DeviceMappings = std::unordered_map<DeviceID, ButtonMap>;

    DeviceMappings loadInputMappings(const std::string& path);
    void saveInputMapping(const DeviceMappings&, const std::string& path);
};
