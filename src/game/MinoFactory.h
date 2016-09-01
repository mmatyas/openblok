#pragma once

#include "Mino.h"
#include "Piece.h"
#include "system/RGBColor.h"

#include <memory>


class MinoFactory {
public:
    static Mino make(Piece::Type);
    static std::unique_ptr<Mino> make_uptr(Piece::Type);
    static RGBColor color(Piece::Type);
};
