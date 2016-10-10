#pragma once

#include "Piece.h"
#include <memory>


class PieceFactory {
public:
    static std::unique_ptr<Piece> make_uptr(PieceType);
};
