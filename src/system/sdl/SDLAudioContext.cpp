#include "SDLAudioContext.h"

#include "SDLMusic.h"
#include "SDLSoundEffect.h"
#include "system/Log.h"

#include <SDL2pp/SDL2pp.hh>
#include <exception>
#include <map>


static constexpr int SDL_MIX_FLAGS = MIX_INIT_OGG
#ifdef SDL2_ENABLE_MP3
    | MIX_INIT_MP3
#endif
#ifdef SDL2_ENABLE_MOD
    | MIX_INIT_MOD
#endif
#ifdef SDL2_ENABLE_FLAC
    | MIX_INIT_FLAC
#endif
;

const std::string LOG_TAG("audio");

SDLAudioContext::SDLAudioContext()
    : audio_loader(SDL_MIX_FLAGS)
    , mixer(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)
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
    static std::map<const std::string, std::weak_ptr<Music>> cache;
    auto item = cache[path].lock();
    if (!item)
        cache[path] = item = std::make_shared<SDLMusic>(SDL2pp::Music(path));
    return item;
}

std::shared_ptr<SoundEffect> SDLAudioContext::loadSound(const std::string& path)
{
    static std::map<const std::string, std::weak_ptr<SoundEffect>> cache;
    auto item = cache[path].lock();
    if (!item)
        cache[path] = item = std::make_shared<SDLSoundEffect>(SDL2pp::Chunk(path));
    return item;
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

void SDLAudioContext::toggleSFXMute()
{
    if (mixer.GetVolume(-1) > 0)
        mixer.SetVolume(-1, 0);
    else
        mixer.SetVolume(-1, MIX_MAX_VOLUME);
}

void SDLAudioContext::toggleMusicMute()
{
    if (mixer.GetMusicVolume() > 0)
        mixer.SetMusicVolume(0);
    else
        mixer.SetMusicVolume(MIX_MAX_VOLUME);
}
