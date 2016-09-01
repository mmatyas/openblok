#include "Piece.h"

#include "MinoFactory.h"

#include <assert.h>


char piecetype_as_ascii(Piece::Type type) {
    switch(type) {
    case Piece::Type::I : return 'I';
    case Piece::Type::J : return 'J';
    case Piece::Type::L : return 'L';
    case Piece::Type::O : return 'O';
    case Piece::Type::S : return 'S';
    case Piece::Type::T : return 'T';
    case Piece::Type::Z : return 'Z';
    }
    assert(false);
}

Piece::Piece(Type type, const std::array<std::bitset<16>, 4>& gridbits)
    : type(type)
    , current_rotation(0)
{
    // fill 4 frames of 4x4 grids
    for (unsigned frame = 0; frame < 4; frame++) {
        for (size_t i = 0; i < 16; i++) {
            if (gridbits[frame].test(15 - i)) {
                grids[frame][i / 4][i % 4] = MinoFactory::make_uptr(type);
            }
        }
    }
}

void Piece::rotateLeft()
{
    // wrap around from right to remain unsigned
    current_rotation = (current_rotation + 3) % 4;
}

void Piece::rotateRight()
{
    current_rotation = (current_rotation + 1) % 4;
}

const PieceGrid& Piece::currentGrid() const
{
    assert(current_rotation < 4);
    return grids.at(current_rotation);
}
