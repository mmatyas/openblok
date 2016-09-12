#pragma once

#include "Matrix.h"
#include "Mino.h"
#include "Piece.h"

#include <chrono>
#include <memory>
#include <vector>
#include <string>
#include <stdint.h>


class AppContext;
class InputEvent;

class GraphicsContext;
namespace SuiteWell {
    class WellFixtureMoveHelper;
    class WellFixtureRotateHelper;
}

class Well {
public:
    /// Create a new well
    Well();

    /// Update the well
    void update(const std::vector<InputEvent>&, AppContext&);

    /// Moves the active piece one row down, if it does not collide.
    void applyGravity();

    /// Add a new, player-controllable piece to the well.
    void addPiece(Piece::Type);
    /// Returns the current, active piece, controllable by the player.
    /// Can return nullptr, eg. during animations.
    /// This function is only for reading the piece information.
    /// For actual input handling, call GameBoard's functions.
    const std::unique_ptr<Piece>& activePiece() const { return active_piece; }

    /// Get the well's string representation.
    /// Can be useful for testing and debugging.
    std::string asAscii();
    /// Set the contents of the well from an Ascii string.
    void fromAscii(const std::string&);

    /// Draw the well
    void draw(GraphicsContext&, unsigned x, unsigned y);

private:
    int8_t active_piece_x;
    uint8_t active_piece_y;
    uint8_t ghost_piece_y;
    std::unique_ptr<Piece> active_piece;

    const std::chrono::steady_clock::duration gravity_update_rate;
    std::chrono::steady_clock::duration gravity_timer;

    // TODO: set dimensions from config
    Matrix<std::unique_ptr<Mino>, 22, 10> matrix;

    void calculateGhostOffset();
    bool hasCollisionAt(int offset_x, unsigned offset_y);

    void moveLeftNow();
    void moveRightNow();
    void moveDownNow();
    void rotateCWNow(); // clockwise
    void rotateCCWNow(); // counter-clockwise

friend class SuiteWell::WellFixtureMoveHelper;
friend class SuiteWell::WellFixtureRotateHelper;
};
