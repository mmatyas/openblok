#include "PieceRain.h"

#include "game/components/Mino.h"
#include "game/components/Piece.h"
#include "game/components/PieceFactory.h"

#include <cmath>
#include <cstdlib>


static const int PADDING_PX = 5;
static const int PIECE_SIDES_PX = PADDING_PX + 4 * Mino::texture_size_px;

namespace Layout {

PieceRain::PieceRain()
    : bottom_y(std::chrono::seconds(4),
               [this](double t) {
                   return this->y() + this->height() + t * PIECE_SIDES_PX; },
               [this](){
                   this->active_pieces.pop_front();
                   this->bottom_y.restart();
               })
{
    bounding_box = {0, 0, PIECE_SIDES_PX, PIECE_SIDES_PX};
}

void PieceRain::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;
}

void PieceRain::setHeight(unsigned height_px)
{
    bounding_box.h = height_px;
    // there are at least two pieces: one sliding out and one sliding in
    displayed_piece_count = 2 + std::ceil(height_px * 1.0 / PIECE_SIDES_PX);
}

void PieceRain::update()
{
    // fill from back if there are too few
    while (active_pieces.size() < displayed_piece_count) {
        const unsigned type_idx = std::rand() % PieceTypeList.size();
        const unsigned rotation_cnt = std::rand() % 4;
        active_pieces.emplace_back(PieceFactory::make_uptr(PieceTypeList.at(type_idx)));
        for (unsigned i = 0; i < rotation_cnt; i++)
            active_pieces.back()->rotateCW();
    }
    // remove from front if there are too many
    while (active_pieces.size() > displayed_piece_count)
        active_pieces.pop_front();

    bottom_y.update(Timing::frame_duration);
}

void PieceRain::draw() const
{
    int piece_y = bottom_y.value();
    for (const auto& piece : active_pieces) {
        piece->draw(x() + PADDING_PX, piece_y + PADDING_PX);
        piece_y -= PIECE_SIDES_PX;
    }
}

} // namespace Layout
