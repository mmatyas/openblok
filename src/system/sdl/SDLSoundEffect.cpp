#include "SDLSoundEffect.h"

#include <assert.h>


SDL2pp::Mixer* SDLSoundEffect::mixer = nullptr;

SDLSoundEffect::SDLSoundEffect(SDL2pp::Chunk&& chunk)
    : chunk(std::move(chunk))
{}

void SDLSoundEffect::playOnce()
{
    assert(mixer);
    mixer->PlayChannel(-1, chunk);
}
