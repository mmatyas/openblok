#include "SDLMusic.h"

#include <assert.h>


SDL2pp::Mixer* SDLMusic::mixer = nullptr;

SDLMusic::SDLMusic(SDL2pp::Music&& music)
    : music(std::move(music))
{}

void SDLMusic::playLoop()
{
    assert(mixer);
    mixer->PlayMusic(music, -1);
}

void SDLMusic::fadeOut(std::chrono::steady_clock::duration duration)
{
    mixer->FadeOutMusic(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}
