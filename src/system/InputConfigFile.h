#pragma once

#include "InputMap.h"


class InputConfigFile {
public:
    std::map<DeviceName, DeviceData> load(const std::string& path);
    void save(const std::map<DeviceName, DeviceData>&, const std::string& path);
};

