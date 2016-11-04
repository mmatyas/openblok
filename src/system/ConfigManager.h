#pragma once

#include "Event.h"

#include <map>
#include <string>
#include <vector>
#include <stdint.h>


class ConfigManager {
public:
    using ScancodeMap = std::map<InputType, std::vector<uint16_t>>;

    ScancodeMap loadInputMapping(const std::string& path);
    void saveInputMapping(const ScancodeMap&, const std::string& path);
};
