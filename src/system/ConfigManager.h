#pragma once

#include "InputEvent.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>


class ConfigManager {
public:
    using InputScancodeMap = std::map<InputType, std::vector<uint16_t>>;

    virtual ~ConfigManager() {}

    virtual InputScancodeMap loadInputMapping(const std::string& scriptfile) = 0;
    virtual void saveInputMapping(const InputScancodeMap&, const std::string& scriptfile) = 0;

private:
    static std::unique_ptr<ConfigManager> create();

friend class AppContext;
};
