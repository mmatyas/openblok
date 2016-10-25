#pragma once

#include <stdint.h>

enum class LineClearType : uint8_t {
    NORMAL,
    TSPIN,
    MINI_TSPIN,
};

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
        struct {
            uint8_t count;
        } harddrop;
        struct {
            uint8_t count;
            LineClearType type;
        } lineclear;
    };

    explicit WellEvent(Type type): type(type) {}
};
