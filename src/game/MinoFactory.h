#pragma once

#include "Mino.h"
#include "system/RGBColor.h"

class MinoFactory {
public:
    static Mino make(Mino::Type);
    static RGBColor color(Mino::Type);
};
