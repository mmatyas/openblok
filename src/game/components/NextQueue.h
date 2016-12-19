#pragma once

#include "PieceType.h"

#include <deque>
#include <memory>


class GraphicsContext;
class Piece;

/// Produces the next piece randomly, and allows to preview
/// the next N pieces.
class NextQueue {
public:
    /// Create a piece queue and allow previewing the next N pieces.
    NextQueue(unsigned displayed_piece_count);
    ~NextQueue();

    /// Pop the top of the queue.
    PieceType next();

    /// Draw the N previewable pieces at (x,y)
    void draw(GraphicsContext&, int x, int y) const;

private:
    static std::deque<PieceType> global_piece_queue;
    std::deque<PieceType>::const_iterator global_queue_it;
    std::deque<PieceType> piece_queue;
    std::array<std::unique_ptr<Piece>, 7> piece_storage;
    unsigned displayed_piece_count;

    // When there are multiple players, we want to provide
    // the same order of pieces for all of them.
    void generate_global_pieces();
    void draw_nth_piece(unsigned i, int x, int y) const;
};
