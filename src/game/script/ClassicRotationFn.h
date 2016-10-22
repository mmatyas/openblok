#pragma once

#include "RotationFn.h"


class ClassicRotationFn : public RotationFn {
public:
    ClassicRotationFn() : RotationFn("Classic rotation system") {}
    std::vector<std::pair<int, int>> call(PieceType, PieceDirection, bool) final {
        return {};
    }
};
