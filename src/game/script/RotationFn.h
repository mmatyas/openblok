#pragma once

#include "game/components/PieceType.h"

#include <vector>


class RotationFn {
public:
    virtual ~RotationFn() {}

    virtual std::vector<std::pair<int, int>> call(PieceType, PieceDirection) = 0;
    std::vector<std::pair<int, int>> operator() (PieceType p, PieceDirection d) {
        return call(p, d);
    };
};
