#pragma once

#include "system/Music.h"

#include "SDL2pp/SDL2pp.hh"

class SDLMusic : public Music {
public:
    SDLMusic(SDL2pp::Music&&);

    void playLoop() final;
    void fadeOut(std::chrono::steady_clock::duration) final;

private:
    static SDL2pp::Mixer* mixer;
    SDL2pp::Music music;

friend class SDLAudioContext;
};
