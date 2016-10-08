#include "MinoFactory.h"

#include "game/components/GameplayResources.h"

#include <assert.h>


Mino MinoFactory::make(Piece::Type type)
{
    using Textures = GameplayResources::Textures;

    switch(type) {
    case Piece::Type::I : return Mino(Textures::MINO_I, 'I');
    case Piece::Type::J : return Mino(Textures::MINO_J, 'J');
    case Piece::Type::L : return Mino(Textures::MINO_L, 'L');
    case Piece::Type::O : return Mino(Textures::MINO_O, 'O');
    case Piece::Type::S : return Mino(Textures::MINO_S, 'S');
    case Piece::Type::T : return Mino(Textures::MINO_T, 'T');
    case Piece::Type::Z : return Mino(Textures::MINO_Z, 'Z');
    default:
        assert(false);
    }
}

std::unique_ptr<Mino> MinoFactory::make_uptr(Piece::Type type)
{
    using Textures = GameplayResources::Textures;

    switch(type) {
    case Piece::Type::I : return std::make_unique<Mino>(Textures::MINO_I, 'I');
    case Piece::Type::J : return std::make_unique<Mino>(Textures::MINO_J, 'J');
    case Piece::Type::L : return std::make_unique<Mino>(Textures::MINO_L, 'L');
    case Piece::Type::O : return std::make_unique<Mino>(Textures::MINO_O, 'O');
    case Piece::Type::S : return std::make_unique<Mino>(Textures::MINO_S, 'S');
    case Piece::Type::T : return std::make_unique<Mino>(Textures::MINO_T, 'T');
    case Piece::Type::Z : return std::make_unique<Mino>(Textures::MINO_Z, 'Z');
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

TextureID MinoFactory::ghostTexture(Piece::Type type)
{
    using Textures = GameplayResources::Textures;

    switch(type) {
    case Piece::Type::I : return Textures::MINO_GHOST_I;
    case Piece::Type::J : return Textures::MINO_GHOST_J;
    case Piece::Type::L : return Textures::MINO_GHOST_L;
    case Piece::Type::O : return Textures::MINO_GHOST_O;
    case Piece::Type::S : return Textures::MINO_GHOST_S;
    case Piece::Type::T : return Textures::MINO_GHOST_T;
    case Piece::Type::Z : return Textures::MINO_GHOST_Z;
    default:
        assert(false);
    }
}
