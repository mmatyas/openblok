#pragma once

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
    unsigned previewCount() const { return displayed_piece_count; }
    ///
    const std::deque<PieceType>& queue() const { return piece_queue; }
    const std::array<std::unique_ptr<Piece>, 7>& piecePtrs() const { return piece_storage; }

private:
    static std::deque<PieceType> global_piece_queue;
    std::deque<PieceType>::const_iterator global_queue_it;
    std::deque<PieceType> piece_queue;
    std::array<std::unique_ptr<Piece>, 7> piece_storage;
    unsigned displayed_piece_count;

    // When there are multiple players, we want to provide
    // the same order of pieces for all of them.
    void generate_global_pieces();
    void fill_queue();
};
