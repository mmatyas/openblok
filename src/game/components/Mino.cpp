#include "Mino.h"

#include <assert.h>


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

void Mino::drawPartial(const Rectangle& from, const Rectangle& to)
{
    assert(texture);
    texture->drawPartialScaled(from, to);
}
