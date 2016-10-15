#include "Piece.h"

#include "MinoStorage.h"

#include <assert.h>


PieceType Piece::typeFromAscii(char type) {
    switch(type) {
    case 'I' : return PieceType::I;
    case 'J' : return PieceType::J;
    case 'L' : return PieceType::L;
    case 'O' : return PieceType::O;
    case 'S' : return PieceType::S;
    case 'T' : return PieceType::T;
    case 'Z' : return PieceType::Z;
    }
    assert(false);
}

uint8_t Piece::displayWidth(PieceType type) {
    switch(type) {
    case PieceType::I : return 4;
    case PieceType::J : return 3;
    case PieceType::L : return 3;
    case PieceType::O : return 4;
    case PieceType::S : return 3;
    case PieceType::T : return 3;
    case PieceType::Z : return 3;
    }
    assert(false);
}

Piece::Piece(PieceType type, const std::array<std::bitset<16>, 4>& gridbits)
    : piece_type(type)
    , current_rotation(0)
{
    // fill 4 frames of 4x4 grids
    for (size_t frame = 0; frame < 4; frame++) {
        for (size_t i = 0; i < 16; i++) {
            if (gridbits[frame].test(15 - i)) {
                grids[frame][i / 4][i % 4] = MinoStorage::getMino(type);
            }
        }
    }
}

void Piece::rotateCCW()
{
    // wrap around from right to remain unsigned
    current_rotation = (current_rotation + 3) % 4;
}

void Piece::rotateCW()
{
    current_rotation = (current_rotation + 1) % 4;
}

const PieceGrid& Piece::currentGrid() const
{
    assert(current_rotation < 4);
    return grids.at(current_rotation);
}

PieceGrid& Piece::currentGridMut()
{
    assert(current_rotation < 4);
    return grids[current_rotation];
}

void Piece::draw(int x, int y)
{
    const auto& frame = currentGrid();
    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            if (frame[row][col])
                frame[row][col]->draw(x + col * Mino::texture_size_px,
                                      y + row * Mino::texture_size_px);
        }
    }
}
