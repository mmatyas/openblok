#include "MinoFactory.h"

#include "Resources.h"

#include <assert.h>


Mino MinoFactory::make(Mino::Type type)
{
    switch(type) {
    case Mino::Type::I : return Mino(TexID::MINO_I, 'I');
    case Mino::Type::J : return Mino(TexID::MINO_J, 'J');
    case Mino::Type::L : return Mino(TexID::MINO_L, 'L');
    case Mino::Type::O : return Mino(TexID::MINO_O, 'O');
    case Mino::Type::S : return Mino(TexID::MINO_S, 'S');
    case Mino::Type::T : return Mino(TexID::MINO_T, 'T');
    case Mino::Type::Z : return Mino(TexID::MINO_Z, 'Z');
    default:
        assert(false);
    }
}

RGBColor MinoFactory::color(Mino::Type type)
{
    switch(type) {
    case Mino::Type::I : return 0x00FFFF_rgb;
    case Mino::Type::J : return 0x0000FF_rgb;
    case Mino::Type::L : return 0xFFA500_rgb;
    case Mino::Type::O : return 0xFFFF00_rgb;
    case Mino::Type::S : return 0x80FF00_rgb;
    case Mino::Type::T : return 0x800080_rgb;
    case Mino::Type::Z : return 0xFF0000_rgb;
    default:
        assert(false);
    }
}
