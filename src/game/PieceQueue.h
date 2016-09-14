#pragma once

#include "Piece.h"

#include <deque>
#include <memory>


class GraphicsContext;

/// Produces the next piece randomly, and allows to preview
/// the next N pieces.
class PieceQueue {
public:
    /// Create a piece queue and allow previewing the next N pieces.
    PieceQueue(unsigned displayed_piece_count);

    /// Pop the top of the queue.
    Piece::Type next();

    /// Draw the N previewable pieces at (x,y)
    void draw(GraphicsContext&, unsigned x, unsigned y);

private:
    std::deque<Piece::Type> piece_queue;
    std::array<std::unique_ptr<Piece>, 7> piece_storage;
    unsigned displayed_piece_count;

    void generate_pieces();
};
