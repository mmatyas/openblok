#include "PieceType.h"

#include <unordered_map>
#include <assert.h>


char toAscii(PieceType type)
{
    static const std::unordered_map<PieceType, char, PieceTypeHash> map = {
        { PieceType::I, 'I' },
        { PieceType::J, 'J' },
        { PieceType::L, 'L' },
        { PieceType::O, 'O' },
        { PieceType::S, 'S' },
        { PieceType::T, 'T' },
        { PieceType::Z, 'Z' },
        { PieceType::GARBAGE, '+' },
    };

    return map.at(type);
}

char toAscii(PieceDirection direction)
{
    static const std::unordered_map<PieceDirection, char, PieceDirectionHash> map = {
        { PieceDirection::NORTH, 'N' },
        { PieceDirection::EAST, 'E' },
        { PieceDirection::SOUTH, 'S' },
        { PieceDirection::WEST, 'W' },
    };
    return map.at(direction);
}

PieceDirection nextCW(PieceDirection direction)
{
    static const std::unordered_map<PieceDirection, PieceDirection, PieceDirectionHash> map = {
        { PieceDirection::NORTH, PieceDirection::EAST },
        { PieceDirection::EAST, PieceDirection::SOUTH },
        { PieceDirection::SOUTH, PieceDirection::WEST },
        { PieceDirection::WEST, PieceDirection::NORTH },
    };
    return map.at(direction);
}

PieceDirection prevCW(PieceDirection direction)
{
    static const std::unordered_map<PieceDirection, PieceDirection, PieceDirectionHash> map = {
        { PieceDirection::NORTH, PieceDirection::WEST },
        { PieceDirection::EAST, PieceDirection::NORTH },
        { PieceDirection::SOUTH, PieceDirection::EAST },
        { PieceDirection::WEST, PieceDirection::SOUTH },
    };
    return map.at(direction);
}
