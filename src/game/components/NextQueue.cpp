#include "NextQueue.h"

#include "PieceDraw.h"
#include "PieceFactory.h"
#include "Mino.h"
#include "system/GraphicsContext.h"

#include <algorithm>
#include <array>
#include <assert.h>


std::deque<PieceType> NextQueue::global_piece_queue = {};

NextQueue::NextQueue(unsigned displayed_piece_count)
    : displayed_piece_count(displayed_piece_count)
{
    // TODO: use a proper random generator

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

NextQueue::~NextQueue()
{
    // NOTE: next queues are created and destructed together
    global_piece_queue.clear();
}

PieceType NextQueue::next()
{
    PieceType piece = piece_queue.front();
    piece_queue.pop_front();
    fill_queue();
    return piece;
}

void NextQueue::generate_global_pieces()
{
    std::array<PieceType, PieceTypeList.size()> possible_pieces = PieceTypeList;
    std::random_shuffle(possible_pieces.begin(), possible_pieces.end());
    for (const auto p : possible_pieces)
        global_piece_queue.push_back(p);
}

void NextQueue::fill_queue()
{
    if (piece_queue.size() <= displayed_piece_count) {
        if (std::distance(global_queue_it, global_piece_queue.cend()) <= static_cast<int>(PieceTypeList.size()))
            generate_global_pieces();

        piece_queue.insert(piece_queue.end(),
                           global_queue_it,
                           global_queue_it + PieceTypeList.size());
        global_queue_it += PieceTypeList.size();
    }
    assert(piece_queue.size() > displayed_piece_count);
}

void NextQueue::setPreviewCount(unsigned num)
{
    displayed_piece_count = num;
    fill_queue();
}
