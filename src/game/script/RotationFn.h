#pragma once

#include "game/components/PieceType.h"

#include <vector>


class RotationFn {
public:
    RotationFn(const std::string& plugin_name) : plugin_name(plugin_name) {}
    virtual ~RotationFn() {}

    const std::string& pluginName() const { return plugin_name; };

    virtual std::vector<std::pair<int, int>> call(PieceType, PieceDirection, bool) = 0;
    std::vector<std::pair<int, int>> operator() (PieceType p, PieceDirection from, bool cw) {
        return call(p, from, cw);
    };

protected:
    std::string plugin_name;
};
