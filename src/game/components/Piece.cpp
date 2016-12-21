#include "Piece.h"

#include "MinoStorage.h"

#include <assert.h>


PieceType Piece::typeFromAscii(char type) {
    static const std::unordered_map<char, PieceType> type_map = {
        {'I', PieceType::I},
        {'J', PieceType::J},
        {'L', PieceType::L},
        {'O', PieceType::O},
        {'S', PieceType::S},
        {'T', PieceType::T},
        {'Z', PieceType::Z},
        {'+', PieceType::GARBAGE},
    };
    assert(type_map.count(type));
    return type_map.at(type);
}

uint8_t Piece::displayWidth(PieceType type) {
    static const std::unordered_map<PieceType, uint8_t, PieceTypeHash> width_map = {
        {PieceType::I, 4},
        {PieceType::J, 3},
        {PieceType::L, 3},
        {PieceType::O, 4},
        {PieceType::S, 3},
        {PieceType::T, 3},
        {PieceType::Z, 3},
    };
    assert(width_map.count(type)); // garbage is not allowed!
    return width_map.at(type);
}

Piece::Piece(PieceType type, const std::array<std::bitset<16>, 4>& gridbits)
    : piece_type(type)
    , current_rotation(PieceDirection::NORTH)
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

void Piece::draw(int x, int y) const
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
