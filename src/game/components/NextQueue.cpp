#include "NextQueue.h"

#include "PieceFactory.h"
#include "system/GraphicsContext.h"

#include <algorithm>
#include <array>
#include <assert.h>


std::deque<PieceType> NextQueue::global_piece_queue = {};

NextQueue::NextQueue (unsigned displayed_piece_count)
    : displayed_piece_count(displayed_piece_count)
{
    if (global_piece_queue.size() <= displayed_piece_count)
        generate_global_pieces();

    global_queue_it = global_piece_queue.cbegin();
    piece_queue = global_piece_queue;

    size_t i = 0;
    for(const auto ptype : PieceTypeList) {
        piece_storage[i] = PieceFactory::make_uptr(ptype);
        i++;
    }
}

NextQueue::~NextQueue() = default;

PieceType NextQueue::next()
{
    PieceType piece = piece_queue.front();
    piece_queue.pop_front();
    if (piece_queue.size() <= displayed_piece_count) {
        if (std::distance(global_queue_it, global_piece_queue.cend()) <= static_cast<int>(PieceTypeList.size()))
            generate_global_pieces();

        piece_queue.insert(piece_queue.end(),
                           global_queue_it,
                           global_queue_it + PieceTypeList.size());
        global_queue_it += PieceTypeList.size();
    }

    assert(piece_queue.size() > displayed_piece_count);
    return piece;
}

void NextQueue::generate_global_pieces()
{
    std::array<PieceType, PieceTypeList.size()> possible_pieces = PieceTypeList;
    std::random_shuffle(possible_pieces.begin(), possible_pieces.end());
    for (const auto p : possible_pieces)
        global_piece_queue.push_back(p);
}

void NextQueue::draw(GraphicsContext& gcx, int x, int y) const
{
    gcx.drawFilledRect({
        x, y,
        5 * Mino::texture_size_px, 4 * Mino::texture_size_px},
        0x0A0AFF80_rgba);

    int offset_y = y + Mino::texture_size_px;
    draw_nth_piece(0, x, offset_y);
    offset_y += Mino::texture_size_px * 3;

    const auto scale = gcx.getDrawScale();
    gcx.modifyDrawScale(scale * 0.75);
    x *= (1 / 0.75);
    offset_y *= (1 / 0.75);
    offset_y += Mino::texture_size_px;
    for (unsigned i = 1; i < displayed_piece_count; i++) {
        draw_nth_piece(i, x, offset_y);
        offset_y += Mino::texture_size_px * 3;
    }
    gcx.modifyDrawScale(scale);
}

void NextQueue::draw_nth_piece(unsigned i, int x, int y) const
{
    assert(i < piece_queue.size());
    assert(i < displayed_piece_count);
    const auto& piece = piece_storage.at(static_cast<size_t>(piece_queue.at(i)));
    const float padding_x = (4 - Piece::displayWidth(piece->type())) / 2.0f;
    piece->draw(x + Mino::texture_size_px * (0.5f + padding_x), y);
}
