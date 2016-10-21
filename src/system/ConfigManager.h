#pragma once

#include "InputEvent.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>


class ConfigManager {
public:
    using ScancodeMap = std::map<InputType, std::vector<uint16_t>>;

    virtual ~ConfigManager() {}

    virtual ScancodeMap loadInputMapping(const std::string& scriptfile) = 0;
    virtual void saveInputMapping(const ScancodeMap&, const std::string& scriptfile) = 0;

private:
    static std::unique_ptr<ConfigManager> create();

friend class AppContext;
};
