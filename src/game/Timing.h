#pragma once

#include <chrono>


using Duration = std::chrono::steady_clock::duration;

namespace Timing {

/// The duration of a frame when the game runs 60 Hz
static constexpr Duration frame_duration_60Hz
    = std::chrono::duration_cast<Duration>(std::chrono::duration<int, std::ratio<1, 60>>(1));

/// The duration of a frame on the current frame rate setting
static constexpr Duration frame_duration
    = std::chrono::duration_cast<Duration>(std::chrono::duration<int, std::ratio<1, 60>>(1));

} // namespace Timing
