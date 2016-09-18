#include "Piece.h"

#include "MinoFactory.h"

#include <assert.h>


const std::array<Piece::Type,7> Piece::allTypes {
    Piece::Type::I,
    Piece::Type::J,
    Piece::Type::L,
    Piece::Type::O,
    Piece::Type::S,
    Piece::Type::T,
    Piece::Type::Z
};

char Piece::typeAsAscii(Piece::Type type) {
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

Piece::Type Piece::typeFromAscii(char type) {
    switch(type) {
    case 'I' : return Piece::Type::I;
    case 'J' : return Piece::Type::J;
    case 'L' : return Piece::Type::L;
    case 'O' : return Piece::Type::O;
    case 'S' : return Piece::Type::S;
    case 'T' : return Piece::Type::T;
    case 'Z' : return Piece::Type::Z;
    }
    assert(false);
}

Piece::Piece(Type type, const std::array<std::bitset<16>, 4>& gridbits)
    : piece_type(type)
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

void Piece::draw(GraphicsContext& gcx, int x, int y)
{
    const auto& frame = currentGrid();
    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            if (frame[row][col])
                frame[row][col]->draw(gcx,
                                      x + col * Mino::texture_size_px,
                                      y + row * Mino::texture_size_px);
        }
    }
}
