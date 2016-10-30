#pragma once

#include "RotationFn.h"


namespace Rotations {

class Classic : public RotationFn {
public:
    Classic();
    std::map<PieceType, std::array<std::bitset<16>, 4>> initialPositions() final;
    std::vector<Offset> possibleOffsets(PieceType, PieceDirection, bool) final {
        return {};
    }
};

}
