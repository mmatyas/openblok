#pragma once

#include "RotationFn.h"

#include <memory>
#include <string>


class ScriptLoader {
public:
    /// A function that returns a vector of (x,y) offsets
    /// based on the piece and the rotation
    std::unique_ptr<RotationFn> loadRotationFn(const std::string& scriptfile);
};
