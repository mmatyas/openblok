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

    /// Pop the top of the queue.
    PieceType next();

    /// Draw the N previewable pieces at (x,y)
    void draw(GraphicsContext&, int x, int y);

private:
    std::deque<PieceType> piece_queue;
    std::array<std::unique_ptr<Piece>, 7> piece_storage;
    unsigned displayed_piece_count;

    void generate_pieces();
};
