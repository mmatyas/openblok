#include "Piece.h"

#include "MinoStorage.h"

#include <assert.h>


Piece::Piece(PieceType type, const std::array<std::bitset<16>, 4>& gridbits)
    : piece_type(type)
    , current_rotation(PieceDirection::NORTH)
{
    // fill 4 frames of 4x4 grids
    for (size_t frame = 0; frame < 4; frame++) {
        for (size_t i = 0; i < 16; i++) {
            grids[frame][i / 4][i % 4] = gridbits[frame].test(15 - i);
        }
    }
}

void Piece::rotateCCW()
{
    current_rotation = prevCW(current_rotation);
}

void Piece::rotateCW()
{
    current_rotation = nextCW(current_rotation);
}

const PieceGrid& Piece::currentGrid() const
{
    return grids.at(static_cast<uint8_t>(current_rotation));
}

PieceGrid& Piece::currentGridMut()
{
    return grids[static_cast<uint8_t>(current_rotation)];
}
