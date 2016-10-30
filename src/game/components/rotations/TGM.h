#pragma once

#include "RotationFn.h"


namespace Rotations {

class TGM : public RotationFn {
public:
    TGM();

    std::vector<Rotations::Offset> possibleOffsets(PieceType, PieceDirection, bool) final;
};

} // namespace Rotations
