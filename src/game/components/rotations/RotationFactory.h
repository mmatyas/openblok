#pragma once

#include "RotationFn.h"
#include "RotationStyle.h"

#include <memory>


class RotationFactory {
public:
    static std::unique_ptr<RotationFn> make(RotationStyle);
};
