#pragma once

#include "RotationFn.h"


namespace Rotations {

class SRS : public RotationFn {
public:
    SRS();

    std::map<PieceType, std::array<std::bitset<16>, 4>> initialPositions() final;
    std::vector<Rotations::Offset> possibleOffsets(PieceType, PieceDirection, bool) final;
};

} // namespace Rotations
