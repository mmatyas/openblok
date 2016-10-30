#pragma once

#include "game/components/PieceType.h"

#include <array>
#include <bitset>
#include <map>
#include <string>
#include <vector>


namespace Rotations {
    struct Offset {
        int x;
        int y;
    };
}

class RotationFn {
public:
    RotationFn(const std::string& rotation_name) : rotation_name(rotation_name) {}
    virtual ~RotationFn() {}

    const std::string& rotationName() const { return rotation_name; };

    virtual std::map<PieceType, std::array<std::bitset<16>, 4>> initialPositions() = 0;

    virtual std::vector<Rotations::Offset> possibleOffsets(PieceType, PieceDirection, bool) = 0;
    std::vector<Rotations::Offset> operator() (PieceType p, PieceDirection from, bool cw) {
        return possibleOffsets(p, from, cw);
    };

protected:
    std::string rotation_name;
};
