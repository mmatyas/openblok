#pragma once

#include "Mino.h"
#include "PieceType.h"
#include "game/util/Matrix.h"

#include <array>
#include <bitset>
#include <memory>
#include <stdint.h>


// 4x4 grid of nullable Minos
typedef Matrix<std::shared_ptr<Mino>, 4, 4> PieceGrid;

/// A Piece is a collection of Minos, that can be controlled as one.
/// It can have specific rotation grids for all four states,
/// and can change between them using rotateLeft/rotateRight.
class Piece {
public:
    static PieceType typeFromAscii(char);
    static uint8_t displayWidth(PieceType);
    PieceType type() const { return piece_type; }
    PieceDirection orientation() const { return current_rotation; }

    /// Create a Piece of type, with the four rotations as bitflags
    Piece(PieceType, const std::array<std::bitset<16>, 4>&);

    /// Rotate the piece clockwise
    void rotateCW();
    /// Rotate the piece counter-clockwise
    void rotateCCW();
    /// Read the rotation grid of the piece
    const PieceGrid& currentGrid() const;
    /// Returns the rotation grid, allowing modifications
    PieceGrid& currentGridMut();

    /// Draw the Piece
    void draw(int x, int y) const;

private:
    const PieceType piece_type;
    PieceDirection current_rotation;
    std::array<PieceGrid, 4> grids;
};
