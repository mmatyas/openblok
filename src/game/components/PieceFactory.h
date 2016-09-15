#pragma once

#include "Piece.h"


class PieceFactory {
public:
    static std::unique_ptr<Piece> make_uptr(Piece::Type);
};
