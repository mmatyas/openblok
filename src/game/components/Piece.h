#pragma once

#include "PieceType.h"
#include "game/util/Matrix.h"

#include <array>
#include <bitset>
#include <stdint.h>

// 4x4 grid of Minos
using PieceGrid = Matrix<bool, 4, 4>;


/// A Piece is a collection of Minos, that can be controlled as one.
/// It can have specific rotation grids for all four states,
/// and can change between them using rotateLeft/rotateRight.
class Piece {
public:
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

    /// Returns the piece's type
    PieceType type() const { return piece_type; }
    /// Returns the piece's orientation
    PieceDirection orientation() const { return current_rotation; }

private:
    const PieceType piece_type;
    PieceDirection current_rotation;
    std::array<PieceGrid, 4> grids;
};
