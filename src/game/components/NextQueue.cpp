#include "NextQueue.h"

#include "PieceDraw.h"
#include "PieceFactory.h"
#include "Mino.h"
#include "system/GraphicsContext.h"

#include <algorithm>
#include <array>
#include <assert.h>


std::deque<PieceType> NextQueue::m_global_piece_queue = {};

NextQueue::NextQueue(unsigned displayed_piece_count)
    : m_displayed_piece_count(displayed_piece_count)
{
    // TODO: use a proper random generator

    if (m_global_piece_queue.size() <= displayed_piece_count)
        generate_global_pieces();

    m_global_queue_it = m_global_piece_queue.cbegin();
    m_piece_queue = m_global_piece_queue;

    size_t i = 0;
    for(const auto ptype : PieceTypeList) {
        m_piece_storage[i] = PieceFactory::make_uptr(ptype);
        i++;
    }
}

NextQueue::~NextQueue()
{
    // NOTE: next queues are created and destructed together
    m_global_piece_queue.clear();
}

PieceType NextQueue::next()
{
    PieceType piece = m_piece_queue.front();
    m_piece_queue.pop_front();
    fill_queue();
    return piece;
}

void NextQueue::generate_global_pieces()
{
    std::array<PieceType, PieceTypeList.size()> possible_pieces = PieceTypeList;
    std::random_shuffle(possible_pieces.begin(), possible_pieces.end());
    for (const auto p : possible_pieces)
        m_global_piece_queue.push_back(p);
}

void NextQueue::fill_queue()
{
    if (m_piece_queue.size() <= m_displayed_piece_count) {
        if (std::distance(m_global_queue_it, m_global_piece_queue.cend()) <= static_cast<int>(PieceTypeList.size()))
            generate_global_pieces();

        m_piece_queue.insert(m_piece_queue.end(),
                           m_global_queue_it,
                           m_global_queue_it + PieceTypeList.size());
        m_global_queue_it += PieceTypeList.size();
    }
    assert(piece_queue.size() > displayed_piece_count);
}

void NextQueue::setPreviewCount(unsigned num)
{
    m_displayed_piece_count = num;
    fill_queue();
}

void NextQueue::draw(GraphicsContext& gcx, int x, int y) const
{
    component_draw.draw(*this, gcx, x, y);
}
