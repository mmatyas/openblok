#pragma once

#include <stdint.h>


struct WellEvent {
    enum class Type: uint8_t {
        PIECE_LOCKED,
        NEXT_REQUESTED,
        HOLD_REQUESTED,
        LINE_CLEAR,
    };

    Type type;
    union {
        struct /* LINE_CLEAR */ {
            uint8_t count;
        };
    };

    explicit WellEvent(Type type): type(type) {}
};
