#pragma once

class Music {
public:
    virtual ~Music() {}

    /// Play the music infinitely, starting from the beginning.
    virtual void playLoop() = 0;
};
