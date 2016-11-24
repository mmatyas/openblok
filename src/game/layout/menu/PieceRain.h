#pragma once

#include "game/Transition.h"
#include "game/layout/Box.h"

#include <list>
#include <memory>


class Piece;


namespace Layout {
class PieceRain : public Box {
public:
    PieceRain();

    void setHeight(unsigned);

    void update();
    void draw() const;

private:
    unsigned displayed_piece_count;
    std::list<std::unique_ptr<Piece>> active_pieces;

    Transition<int> bottom_y;
};
} // namespace Layout
