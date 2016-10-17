#include "SDLMusic.h"


SDL2pp::Mixer* SDLMusic::mixer = nullptr;

SDLMusic::SDLMusic(SDL2pp::Music&& music)
    : music(std::move(music))
{}

void SDLMusic::playLoop()
{
    assert(mixer);
    mixer->PlayMusic(music, -1);
}
