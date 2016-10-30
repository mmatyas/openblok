#pragma once

#include "RotationFn.h"


namespace Rotations {

class TGM : public RotationFn {
public:
    TGM();

    std::map<PieceType, std::array<std::bitset<16>, 4>> initialPositions() final;
    std::vector<Rotations::Offset> possibleOffsets(PieceType, PieceDirection, bool) final;
};

} // namespace Rotations
