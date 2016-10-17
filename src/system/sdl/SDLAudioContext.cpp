#include "SDLAudioContext.h"

#include "SDLMusic.h"
#include "SDLSoundEffect.h"
#include "system/Log.h"

#include "SDL2pp/SDL2pp.hh"
#include <exception>


const std::string LOG_TAG("audio");

SDLAudioContext::SDLAudioContext()
    : mixer(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096)
{
    SDLMusic::mixer = &mixer;
    SDLSoundEffect::mixer = &mixer;
}

SDLAudioContext::~SDLAudioContext()
{
    SDLMusic::mixer = nullptr;
    SDLSoundEffect::mixer = nullptr;
}

std::shared_ptr<Music> SDLAudioContext::loadMusic(const std::string& path)
{
    return std::make_shared<SDLMusic>(SDL2pp::Music(path));
}

std::shared_ptr<SoundEffect> SDLAudioContext::loadSound(const std::string& path)
{
    return std::make_shared<SDLSoundEffect>(SDL2pp::Chunk(path));
}

void SDLAudioContext::pauseAll()
{
    mixer.PauseMusic();
    mixer.PauseChannel(-1);
}

void SDLAudioContext::resumeAll()
{
    mixer.ResumeMusic();
    mixer.ResumeChannel(-1);
}

void SDLAudioContext::stopAll()
{
    mixer.HaltMusic();
    mixer.HaltChannel(-1);
}
