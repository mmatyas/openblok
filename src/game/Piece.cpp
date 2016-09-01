#include "Piece.h"

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

Piece::Piece(Type type)
    : type(type)
    , current_rotation(0)
{
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
