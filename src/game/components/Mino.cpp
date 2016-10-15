#include "Mino.h"

#include <assert.h>


// TODO: load from config maybe
const int8_t Mino::texture_size_px = 32;

Mino::Mino(std::unique_ptr<Texture> texture, char ascii_val)
    : texture(std::move(texture))
    , ascii_val(ascii_val)
{
}

void Mino::draw(int x, int y)
{
    assert(texture);
    texture->drawScaled({x, y, texture_size_px, texture_size_px});
}
