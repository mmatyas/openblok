#pragma once

#include "game/script/ClassicRotationFn.h"

#include <memory>


// All values are in number of frames (60Hz)
struct WellConfig {
    unsigned short starting_gravity;
    unsigned short shift_normal;
    unsigned short shift_turbo;
    unsigned short shift_activate_das;
    bool instant_harddrop;
    bool infinity_lock;
    unsigned short lock_delay;
    std::unique_ptr<RotationFn> rotation_fn;

    WellConfig() {
        starting_gravity = 64,
        shift_normal = 14,
        shift_turbo = 4,
        shift_activate_das = 14,
        instant_harddrop = true,
        infinity_lock = true,
        lock_delay = 30,
        rotation_fn = std::make_unique<ClassicRotationFn>();
    };
};
