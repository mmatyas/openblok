#pragma once

struct SysConfig {
    bool fullscreen;
    bool sfx;
    bool music;

    SysConfig()
        : fullscreen(false)
        , sfx(true)
        , music(true)
    {}
};
