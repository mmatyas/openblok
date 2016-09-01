#include "Mino.h"

#include "system/GraphicsContext.h"

// TODO: load from config maybe
const int8_t Mino::texture_size_px = 24;

Mino::Mino(TexID slot, char ascii_val)
    : texture(slot)
    , ascii_val(ascii_val)
{}

void Mino::draw(GraphicsContext& gcx, int x, int y)
{
    gcx.drawTexture(texture, {x, y, texture_size_px, texture_size_px});
}
