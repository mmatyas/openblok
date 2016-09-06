#pragma once

#include "Matrix.h"
#include "Mino.h"

#include <memory>
#include <string>
#include <stdint.h>


class GraphicsContext;

class GameBoard {
public:
    /// Create a new game board
    GameBoard();

    /// Get the game board representation as a string.
    /// Can be useful for testing and debugging.
    std::string asAscii();

    /// Draw the game board
    void draw(GraphicsContext&, unsigned x, unsigned y);

private:
    // TODO: set dimensions from config
    Matrix<std::unique_ptr<Mino>, 22, 10> matrix;
};
