#pragma once

#include "Piece.h"

#include <map>
#include <memory>


class PieceFactory {
public:
    static void changeInitialPositions(std::map<PieceType, std::array<std::bitset<16>, 4>>&&);
    static std::unique_ptr<Piece> make_uptr(PieceType);

private:
    static std::map<PieceType, std::array<std::bitset<16>, 4>> initial_positions;
};
