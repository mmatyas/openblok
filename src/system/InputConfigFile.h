#pragma once

#include "InputMap.h"


class InputConfigFile {
public:
    DeviceMaps load(const std::string& path);
    void save(const DeviceMaps&, const std::string& path);
};

