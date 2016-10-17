#pragma once

#include "system/AudioContext.h"

#include "SDL2pp/SDL2pp.hh"


class SDLAudioContext : public AudioContext {
public:
    SDLAudioContext();
    ~SDLAudioContext();

    std::shared_ptr<Music> loadMusic(const std::string& path) final;
    std::shared_ptr<SoundEffect> loadSound(const std::string& path) final;

    void pauseAll() final;
    void resumeAll() final;
    void stopAll() final;

private:
    SDL2pp::Mixer mixer;
};
