#pragma once

#include <stdint.h>


struct WellEvent {
    enum class Type: uint8_t {
        PIECE_LOCKED,
        PIECE_ROTATED,
        NEXT_REQUESTED,
        HOLD_REQUESTED,
        LINE_CLEAR_ANIMATION_START,
        LINE_CLEAR,
        TSPIN_DETECTED,
        MINI_TSPIN_DETECTED,
        SOFTDROPPED,
        HARDDROPPED,
        GAME_OVER,
    };

    Type type;
    union {
        struct /* LINE_CLEAR, HARDDROPPED */ {
            uint8_t count;
        };
    };

    explicit WellEvent(Type type): type(type) {}
};
