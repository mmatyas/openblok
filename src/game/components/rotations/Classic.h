#pragma once

#include "RotationFn.h"


namespace Rotations {

class Classic : public RotationFn {
public:
    Classic() : RotationFn("Classic rotation system") {}
    std::vector<Offset> possibleOffsets(PieceType, PieceDirection, bool) final {
        return {};
    }
};

}
