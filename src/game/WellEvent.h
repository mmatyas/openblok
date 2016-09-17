#pragma once

#include <stdint.h>


enum class WellEvent: uint8_t {
    PIECE_LOCKED,
    NEXT_REQUESTED,
    HOLD_REQUESTED,
    CLEARED_ONE_LINE,
    CLEARED_TWO_LINES,
    CLEARED_THREE_LINES,
    CLEARED_FOUR_LINES,
};
