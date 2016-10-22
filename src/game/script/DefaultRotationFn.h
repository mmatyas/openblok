#pragma once

#include "RotationFn.h"


class DefaultRotationFn : public RotationFn {
public:
    std::vector<std::pair<int, int>> call(PieceType, PieceDirection) final {
        return {};
    }
};
