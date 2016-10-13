#pragma once

#include "PieceType.h"
#include "game/Transition.h"

#include <array>
#include <memory>


class GraphicsContext;
class Piece;


/// A piece holder, allows swapping the active piece once in every turn.
class HoldQueue {
public:
    HoldQueue();
    ~HoldQueue();

    /// Notify the holder that someone would like to swap.
    void onSwapRequested();
    /// Notify the holder that the current piece has landed, the lines were cleared
    /// and a new piece has arrived, so it can now accept swapping.
    void onNextTurn();

    /// True if there was no swapping yet in the current turn.
    bool swapAllowed() const { return swap_allowed; }

    /// True, if the holder is empty.
    bool isEmpty() const { return empty; }

    /// Returns the currently held piece, and replaces it with the specified one.
    PieceType swapWith(PieceType);
    void swapWithEmpty(PieceType);

    /// Update the animations
    void update();

    /// Draw the current holded piece at (x,y), if any.
    void draw(GraphicsContext&, int x, int y) const;

private:
    bool swap_allowed;
    bool empty;
    PieceType current_piece;
    std::array<std::unique_ptr<Piece>, 7> piece_storage;

    Transition<uint8_t> swapblocked_alpha;
};
