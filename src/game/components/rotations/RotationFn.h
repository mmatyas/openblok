#pragma once

#include "game/components/PieceType.h"

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
    RotationFn(const std::string& plugin_name) : plugin_name(plugin_name) {}
    virtual ~RotationFn() {}

    const std::string& pluginName() const { return plugin_name; };

    virtual std::vector<Rotations::Offset> possibleOffsets(PieceType, PieceDirection, bool) = 0;
    std::vector<Rotations::Offset> operator() (PieceType p, PieceDirection from, bool cw) {
        return possibleOffsets(p, from, cw);
    };

protected:
    std::string plugin_name;
};
