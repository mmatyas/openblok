#pragma once

#include "RotationFn.h"


namespace Rotations {

class SRS : public RotationFn {
public:
    SRS();

    std::vector<Rotations::Offset> possibleOffsets(PieceType, PieceDirection, bool) final;
};

} // namespace Rotations
