#pragma once

class SoundEffect {
public:
    virtual ~SoundEffect() {}

    /// Play the sound effect once from the beginning.
    virtual void playOnce() = 0;
};
