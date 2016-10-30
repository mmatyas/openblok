#include "SRS.h"

namespace Rotations {

SRS::SRS()
    : RotationFn("SRS rotation system")
{}

std::map<PieceType, std::array<std::bitset<16>, 4>> SRS::initialPositions()
{
    //                                   AAAABBBBCCCCDDDD
    static const std::map<PieceType, std::array<std::bitset<16>, 4>> fields = {
        {PieceType::I, {std::bitset<16>("0000111100000000"),
                        std::bitset<16>("0010001000100010"),
                        std::bitset<16>("0000000011110000"),
                        std::bitset<16>("0100010001000100")}},
        {PieceType::J, {std::bitset<16>("1000111000000000"),
                        std::bitset<16>("0110010001000000"),
                        std::bitset<16>("0000111000100000"),
                        std::bitset<16>("0100010011000000")}},
        {PieceType::L, {std::bitset<16>("0010111000000000"),
                        std::bitset<16>("0100010001100000"),
                        std::bitset<16>("0000111010000000"),
                        std::bitset<16>("1100010001000000")}},
        {PieceType::O, {std::bitset<16>("0110011000000000"),
                        std::bitset<16>("0110011000000000"),
                        std::bitset<16>("0110011000000000"),
                        std::bitset<16>("0110011000000000")}},
        {PieceType::S, {std::bitset<16>("0110110000000000"),
                        std::bitset<16>("0100011000100000"),
                        std::bitset<16>("0000011011000000"),
                        std::bitset<16>("1000110001000000")}},
        {PieceType::T, {std::bitset<16>("0100111000000000"),
                        std::bitset<16>("0100011001000000"),
                        std::bitset<16>("0000111001000000"),
                        std::bitset<16>("0100110001000000")}},
        {PieceType::Z, {std::bitset<16>("1100011000000000"),
                        std::bitset<16>("0010011001000000"),
                        std::bitset<16>("0000110001100000"),
                        std::bitset<16>("0100110010000000")}},
    };

    return fields;
}

std::vector<Rotations::Offset> SRS::possibleOffsets(PieceType piece, PieceDirection from, bool clockwise)
{
    std::vector<Rotations::Offset> output;

    if (piece == PieceType::O)
        return output;

    if (piece == PieceType::I) {
        switch (from) {
            case PieceDirection::NORTH:
            case PieceDirection::SOUTH:
                if (clockwise) {
                    output = {
                        {-2,  0},
                        { 1,  0},
                        {-2,  1},
                        { 1, -2},
                    };
                }
                else {
                    output = {
                        {-1,  0},
                        { 2,  0},
                        {-1, -2},
                        { 2,  1},
                    };
                }
                break;
            case PieceDirection::EAST:
            case PieceDirection::WEST:
                if (clockwise) {
                    output = {
                        {-1,  0},
                        { 2,  0},
                        {-1, -2},
                        { 2,  1},
                    };
                }
                else {
                    output = {
                        { 2,  0},
                        {-1,  0},
                        { 2, -1},
                        {-1,  2},
                    };
                }
                break;
        }
        // south mirrors north and west mirrors east on all coords
        if (from == PieceDirection::SOUTH || from == PieceDirection::WEST) {
            for (auto& item : output) {
                item.x *= -1;
                item.y *= -1;
            }
        }

        return output;
    }

    // not I or O pieces

    int xpos = 0;
    switch (from) {
        case PieceDirection::NORTH:
            xpos = clockwise ? -1 : 1;
            output.push_back({xpos, 0});
            output.push_back({xpos, -1});
            if (piece != PieceType::T) output.push_back({0, 2});
            output.push_back({xpos, 2});
            break;
        case PieceDirection::SOUTH:
            xpos = clockwise ? 1 : -1;
            output.push_back({xpos, 0});
            if (piece != PieceType::T) output.push_back({xpos, -1});
            output.push_back({0, 2});
            output.push_back({xpos, 2});
            break;
        case PieceDirection::EAST:
        case PieceDirection::WEST:
            xpos = (from == PieceDirection::EAST) ? 1 : -1;
            output = {
                {xpos,  0},
                {xpos,  1},
                {   0, -2},
                {xpos, -2},
            };
            break;
    }

    return output;
}

} // namespace Rotations
