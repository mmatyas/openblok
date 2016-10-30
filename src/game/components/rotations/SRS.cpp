#include "SRS.h"

namespace Rotations {

SRS::SRS()
    : RotationFn("SRS rotation system")
{}

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
