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

PieceType typeFromAscii(char type) {
    static const std::unordered_map<char, PieceType> type_map = {
        // uppercase
        {'I', PieceType::I},
        {'J', PieceType::J},
        {'L', PieceType::L},
        {'O', PieceType::O},
        {'S', PieceType::S},
        {'T', PieceType::T},
        {'Z', PieceType::Z},
        // garbage
        {'+', PieceType::GARBAGE},
    };
    assert(type_map.count(type));
    return type_map.at(type);
}

uint8_t displayWidth(PieceType type) {
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
