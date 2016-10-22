#pragma once

#include "game/script/RotationFn.h"

#include "sol.hpp"


class LuaRotationFn : public RotationFn {
public:
    LuaRotationFn(const std::string& scriptfile);

    std::vector<std::pair<int, int>> call(PieceType, PieceDirection, bool) final;

private:
    sol::state lua;
    sol::protected_function fn;
};
