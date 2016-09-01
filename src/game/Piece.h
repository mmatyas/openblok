#pragma once

#include "Mino.h"

#include <array>
#include <memory>
#include <stdint.h>


typedef std::array<std::unique_ptr<Mino>, 4> PieceGrid;

/// A Piece is a collection of Minos, that can be controlled as one.
/// It can have specific rotation grids for all four states,
/// and can change between them using rotateLeft/rotateRight.
class Piece {
public:
    /// The Piece types in the game
    enum class Type {
        I, J, L, O, S, T, Z
    };

    /// Create a Piece of type
    Piece(Type);

    /// Rotate the piece left
    void rotateLeft();
    /// Rotate the piece right
    void rotateRight();
    /// Read the rotation grid of the piece
    const PieceGrid& currentGrid() const;

private:
    const Type type;
    uint8_t current_rotation;
    std::array<std::array<std::unique_ptr<Mino>, 4>, 4> grids;
};
