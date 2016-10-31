#pragma once

#include <chrono>


class Music {
public:
    virtual ~Music() {}

    /// Play the music infinitely, starting from the beginning.
    virtual void playLoop() = 0;

    /// Fade out over the provided duration, then stop the music.
    virtual void fadeOut(std::chrono::steady_clock::duration) = 0;
};
