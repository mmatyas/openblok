#pragma once

#include "Mino.h"
#include "game/Matrix.h"

#include <array>
#include <bitset>
#include <memory>
#include <stdint.h>


// 4x4 grid of nullable Minos
typedef Matrix<std::unique_ptr<Mino>, 4, 4> PieceGrid;

/// A Piece is a collection of Minos, that can be controlled as one.
/// It can have specific rotation grids for all four states,
/// and can change between them using rotateLeft/rotateRight.
class Piece {
public:
    /// The Piece types in the game
    enum class Type : uint8_t {
        I, J, L, O, S, T, Z
    };

    static const std::array<Piece::Type, 7> allTypes;

    static char typeAsAscii(Type);
    static Type typeFromAscii(char);
    static uint8_t displayWidth(Type);
    Type type() const { return piece_type; }

    /// Create a Piece of type, with the four rotations as bitflags
    Piece(Type, const std::array<std::bitset<16>, 4>&);

    /// Rotate the piece clockwise
    void rotateCW();
    /// Rotate the piece counter-clockwise
    void rotateCCW();
    /// Read the rotation grid of the piece
    const PieceGrid& currentGrid() const;
    /// Returns the rotation grid, allowing modifications
    PieceGrid& currentGridMut();

    /// Draw the Piece
    void draw(GraphicsContext&, int x, int y);

private:
    const Type piece_type;
    uint8_t current_rotation;
    std::array<PieceGrid, 4> grids;
};
