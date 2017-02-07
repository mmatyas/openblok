#pragma once

#include <string>


struct SysConfig {
    bool fullscreen;
    bool sfx;
    bool music;
    std::string theme_dir_name;

    SysConfig()
        : fullscreen(false)
        , sfx(true)
        , music(true)
        , theme_dir_name("default")
    {}
};
