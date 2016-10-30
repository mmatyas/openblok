#include "TGM.h"


namespace Rotations {

TGM::TGM()
    : RotationFn("TGM rotation system")
{}

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
