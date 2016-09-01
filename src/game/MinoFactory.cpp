#include "MinoFactory.h"

#include "Resources.h"

#include <assert.h>


Mino MinoFactory::make(Mino::Type type)
{
    switch(type) {
    case Mino::Type::I : return Mino(type, TexID::MINO_I);
    case Mino::Type::J : return Mino(type, TexID::MINO_J);
    case Mino::Type::L : return Mino(type, TexID::MINO_L);
    case Mino::Type::O : return Mino(type, TexID::MINO_O);
    case Mino::Type::S : return Mino(type, TexID::MINO_S);
    case Mino::Type::T : return Mino(type, TexID::MINO_T);
    case Mino::Type::Z : return Mino(type, TexID::MINO_Z);
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
