#pragma once

#include "NextQueueDraw.h"
#include "PieceType.h"
#include "system/Color.h"

#include <deque>
#include <memory>

class GraphicsContext;
class Piece;


/// Produces the next piece randomly, and allows to preview
/// the next N pieces.
class NextQueue {
public:
    /// Create a piece queue and allow previewing the next N pieces.
    NextQueue(unsigned displayed_piece_count = 1);
    ~NextQueue();

    /// Pop the top of the queue.
    PieceType next();
    void setPreviewCount(unsigned); // TODO remove
    unsigned previewCount() const { return m_displayed_piece_count; }

    void draw(GraphicsContext&, int x, int y) const;

// for components
    std::deque<PieceType> m_piece_queue;
    std::array<std::unique_ptr<Piece>, 7> m_piece_storage;
    unsigned m_displayed_piece_count;

private:
    static std::deque<PieceType> m_global_piece_queue;
    std::deque<PieceType>::const_iterator m_global_queue_it;

    // When there are multiple players, we want to provide
    // the same order of pieces for all of them.
    void generate_global_pieces();
    void fill_queue();

    NextQueueDraw component_draw;
};
