#pragma once

#include "Mino.h"
#include "Piece.h"
#include "system/RGBColor.h"

class MinoFactory {
public:
    static Mino make(Piece::Type);
    static RGBColor color(Piece::Type);
};
