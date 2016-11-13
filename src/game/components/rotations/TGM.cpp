#include "TGM.h"


namespace Rotations {

TGM::TGM()
    : RotationFn("TGM rotation system")
{}

std::map<PieceType, std::array<std::bitset<16>, 4>> TGM::initialPositions()
{
    //                                    AAAABBBBCCCCDDDD
    static const std::map<PieceType, std::array<std::bitset<16>, 4>> fields = {
        {PieceType::I, {{std::bitset<16>("0000111100000000"),
                         std::bitset<16>("0010001000100010"),
                         std::bitset<16>("0000111100000000"),
                         std::bitset<16>("0010001000100010")}}},
        {PieceType::J, {{std::bitset<16>("0000111000100000"),
                         std::bitset<16>("0100010011000000"),
                         std::bitset<16>("0000100011100000"),
                         std::bitset<16>("0110010001000000")}}},
        {PieceType::L, {{std::bitset<16>("0000111010000000"),
                         std::bitset<16>("1100010001000000"),
                         std::bitset<16>("0000001011100000"),
                         std::bitset<16>("0100010001100000")}}},
        {PieceType::O, {{std::bitset<16>("0110011000000000"),
                         std::bitset<16>("0110011000000000"),
                         std::bitset<16>("0110011000000000"),
                         std::bitset<16>("0110011000000000")}}},
        {PieceType::S, {{std::bitset<16>("0000011011000000"),
                         std::bitset<16>("1000110001000000"),
                         std::bitset<16>("0000011011000000"),
                         std::bitset<16>("1000110001000000")}}},
        {PieceType::T, {{std::bitset<16>("0000111001000000"),
                         std::bitset<16>("0100110001000000"),
                         std::bitset<16>("0000010011100000"),
                         std::bitset<16>("0100011001000000")}}},
        {PieceType::Z, {{std::bitset<16>("0000110001100000"),
                         std::bitset<16>("0010011001000000"),
                         std::bitset<16>("0000110001100000"),
                         std::bitset<16>("0010011001000000")}}},
    };

    return fields;
}

std::vector<Rotations::Offset> TGM::possibleOffsets(PieceType piece, PieceDirection, bool)
{
    // try 1 tile right, left, right-up (floor kick), left-up (floor kick)
    std::vector<Rotations::Offset> output = {
        {1, 0},
        {-1, 0},
        {1, -1},
        {-1, -1},
    };

    if (piece == PieceType::I) {
        std::vector<Rotations::Offset> i_offsets = {
            {2, 0},
            {-2, 0},
            {2, -1},
            {-2, -1},
            {2, -2},
            {-2, -2},
        };
        output.insert(output.end(), i_offsets.begin(), i_offsets.end());
    }

    return output;
}

} // namespace Rotations
