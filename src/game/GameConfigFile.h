#pragma once

#include "SysConfig.h"
#include "WellConfig.h"

#include <string>
#include <tuple>


class GameConfigFile {
public:
    static void save(SysConfig&, WellConfig&, const std::string& path);
    static std::tuple<SysConfig, WellConfig> load(const std::string& path);
};
