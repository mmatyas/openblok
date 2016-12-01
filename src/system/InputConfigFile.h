#pragma once

#include "InputMap.h"


class InputConfigFile {
public:
    DeviceMap load(const std::string& path);
    void save(const DeviceMap&, const std::string& path);
};

