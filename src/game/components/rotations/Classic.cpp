#include "Classic.h"

namespace Rotations {

Classic::Classic()
    : RotationFn("Classic rotation system")
{}

std::map<PieceType, std::array<std::bitset<16>, 4>> Classic::initialPositions()
{
    //                                   AAAABBBBCCCCDDDD
    static const std::map<PieceType, std::array<std::bitset<16>, 4>> fields = {
        {PieceType::I, {std::bitset<16>("0000000011110000"),
                        std::bitset<16>("0010001000100010"),
                        std::bitset<16>("0000000011110000"),
                        std::bitset<16>("0010001000100010")}},
        {PieceType::J, {std::bitset<16>("0000111000100000"),
                        std::bitset<16>("0100010011000000"),
                        std::bitset<16>("1000111000000000"),
                        std::bitset<16>("0110010001000000")}},
        {PieceType::L, {std::bitset<16>("0000111010000000"),
                        std::bitset<16>("1100010001000000"),
                        std::bitset<16>("0010111000000000"),
                        std::bitset<16>("0100010001100000")}},
        {PieceType::O, {std::bitset<16>("0110011000000000"),
                        std::bitset<16>("0110011000000000"),
                        std::bitset<16>("0110011000000000"),
                        std::bitset<16>("0110011000000000")}},
        {PieceType::S, {std::bitset<16>("0000011011000000"),
                        std::bitset<16>("0100011000100000"),
                        std::bitset<16>("0000011011000000"),
                        std::bitset<16>("0100011000100000")}},
        {PieceType::T, {std::bitset<16>("0000111001000000"),
                        std::bitset<16>("0100110001000000"),
                        std::bitset<16>("0100111000000000"),
                        std::bitset<16>("0100011001000000")}},
        {PieceType::Z, {std::bitset<16>("0000110001100000"),
                        std::bitset<16>("0010011001000000"),
                        std::bitset<16>("0000110001100000"),
                        std::bitset<16>("0010011001000000")}},
    };

    return fields;
}

} // namespace Rotations
