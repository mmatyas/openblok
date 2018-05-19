#include "PieceDraw.h"

#include "Mino.h"
#include "MinoStorage.h"


void drawPiece(const Piece& piece, int x, int y)
{
    const auto& mino = MinoStorage::getMino(piece.type());
    const auto& frame = piece.currentGrid();

    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            if (frame[row][col])
                mino->draw(x + col * Mino::texture_size_px,
                           y + row * Mino::texture_size_px);
        }
    }
}
