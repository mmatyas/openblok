#include "MinoFactory.h"

#include "Resources.h"

#include <assert.h>


Mino MinoFactory::make(Piece::Type type)
{
    switch(type) {
    case Piece::Type::I : return Mino(TexID::MINO_I, 'I');
    case Piece::Type::J : return Mino(TexID::MINO_J, 'J');
    case Piece::Type::L : return Mino(TexID::MINO_L, 'L');
    case Piece::Type::O : return Mino(TexID::MINO_O, 'O');
    case Piece::Type::S : return Mino(TexID::MINO_S, 'S');
    case Piece::Type::T : return Mino(TexID::MINO_T, 'T');
    case Piece::Type::Z : return Mino(TexID::MINO_Z, 'Z');
    default:
        assert(false);
    }
}

std::unique_ptr<Mino> MinoFactory::make_uptr(Piece::Type type)
{
    switch(type) {
    case Piece::Type::I : return std::make_unique<Mino>(TexID::MINO_I, 'I');
    case Piece::Type::J : return std::make_unique<Mino>(TexID::MINO_J, 'J');
    case Piece::Type::L : return std::make_unique<Mino>(TexID::MINO_L, 'L');
    case Piece::Type::O : return std::make_unique<Mino>(TexID::MINO_O, 'O');
    case Piece::Type::S : return std::make_unique<Mino>(TexID::MINO_S, 'S');
    case Piece::Type::T : return std::make_unique<Mino>(TexID::MINO_T, 'T');
    case Piece::Type::Z : return std::make_unique<Mino>(TexID::MINO_Z, 'Z');
    default:
        assert(false);
    }
}

RGBColor MinoFactory::color(Piece::Type type)
{
    switch(type) {
    case Piece::Type::I : return 0x00FFFF_rgb;
    case Piece::Type::J : return 0x0000FF_rgb;
    case Piece::Type::L : return 0xFFA500_rgb;
    case Piece::Type::O : return 0xFFFF00_rgb;
    case Piece::Type::S : return 0x80FF00_rgb;
    case Piece::Type::T : return 0xAA00FF_rgb;
    case Piece::Type::Z : return 0xFF0000_rgb;
    default:
        assert(false);
    }
}
