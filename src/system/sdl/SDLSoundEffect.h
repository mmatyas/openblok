#pragma once

#include "system/SoundEffect.h"

#include "SDL2pp/SDL2pp.hh"


class SDLSoundEffect : public SoundEffect {
public:
    SDLSoundEffect(SDL2pp::Chunk&&);

    void playOnce() final;

private:
    static SDL2pp::Mixer* mixer;
    SDL2pp::Chunk chunk;

friend class SDLAudioContext;
};
