#include "PieceType.h"

#include <assert.h>


char toAscii(PieceType type)
{
    switch(type) {
        case PieceType::I: return 'I';
        case PieceType::J: return 'J';
        case PieceType::L: return 'L';
        case PieceType::O: return 'O';
        case PieceType::S: return 'S';
        case PieceType::T: return 'T';
        case PieceType::Z: return 'Z';
        case PieceType::GARBAGE: return '+';
    }
    assert(false);
}

char toAscii(PieceDirection direction)
{
    switch(direction) {
        case PieceDirection::NORTH: return 'N';
        case PieceDirection::EAST: return 'E';
        case PieceDirection::SOUTH: return 'S';
        case PieceDirection::WEST: return 'W';
    }
    assert(false);
}

PieceDirection nextCW(PieceDirection direction)
{
    switch(direction) {
        case PieceDirection::NORTH: return PieceDirection::EAST;
        case PieceDirection::EAST: return PieceDirection::SOUTH;
        case PieceDirection::SOUTH: return PieceDirection::WEST;
        case PieceDirection::WEST: return PieceDirection::NORTH;
    }
    assert(false);
}

PieceDirection prevCW(PieceDirection direction)
{
    switch(direction) {
        case PieceDirection::NORTH: return PieceDirection::WEST;
        case PieceDirection::EAST: return PieceDirection::NORTH;
        case PieceDirection::SOUTH: return PieceDirection::EAST;
        case PieceDirection::WEST: return PieceDirection::SOUTH;
    }
    assert(false);
}
