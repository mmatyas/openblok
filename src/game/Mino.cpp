#include "Mino.h"

#include "system/GraphicsContext.h"

#include <assert.h>


char minotype_as_ascii(Mino::Type type) {
    switch(type) {
    case Mino::Type::I : return 'I';
    case Mino::Type::J : return 'J';
    case Mino::Type::L : return 'L';
    case Mino::Type::O : return 'O';
    case Mino::Type::S : return 'S';
    case Mino::Type::T : return 'T';
    case Mino::Type::Z : return 'Z';
    }
    assert(false);
}

// TODO: load from config maybe
const int8_t Mino::texture_size_px = 24;

Mino::Mino(Type type, TexID tex)
    : type(type)
    , texture(tex)
    , ascii_val(minotype_as_ascii(type))
{
}

void Mino::draw(GraphicsContext& gcx, int x, int y)
{
    gcx.drawTexture(texture, {x, y, texture_size_px, texture_size_px});
}
