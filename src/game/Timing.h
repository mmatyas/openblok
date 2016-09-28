#pragma once

#include <chrono>


using Duration = std::chrono::steady_clock::duration;

/// The duration of a frame when the game runs 60 Hz
static constexpr Duration frame_duration_60Hz
    = std::chrono::duration_cast<Duration>(std::chrono::duration<int, std::ratio<1, 60>>(1));
