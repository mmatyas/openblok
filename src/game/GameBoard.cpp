#include "GameBoard.h"

#include "PieceFactory.h"
#include "Resources.h"
#include "system/GraphicsContext.h"

#include <algorithm>
#include <assert.h>


GameBoard::GameBoard()
    : active_piece_x(0)
    , active_piece_y(0)
    , ghost_piece_y(0)
{
}

void GameBoard::addPiece(Piece::Type type)
{
    // the player can only control one piece at a time
    assert(!active_piece);

    active_piece = PieceFactory::make_uptr(type);
    active_piece_x = 3;
    active_piece_y = 0;
    calculateGhostOffset();
}

std::string GameBoard::asAscii()
{
    // the piece must be inside the grid
    assert(active_piece_x + 3 < matrix[0].size());
    assert(active_piece_y + 3 < matrix.size());

    std::string output;
    for (size_t row = 0; row < matrix.size(); row++) {
        for (size_t cell = 0; cell < matrix[row].size(); cell++) {
            if (active_piece
                && active_piece_x <= cell && cell < active_piece_x + 4
                && active_piece_y <= row && row < active_piece_y + 4) {
                const auto& mino = active_piece->currentGrid().at(row - active_piece_y)
                                                              .at(cell - active_piece_x);
                if (mino) {
                    output += std::tolower(mino->asAscii());
                    continue;
                }
            }

            if (matrix[row][cell])
                output += matrix[row][cell]->asAscii();
            else
                output += '.';
        }
        output += '\n';
    }
    return output;
}

void GameBoard::draw(GraphicsContext& gcx, unsigned int x, unsigned int y)
{
    // Draw background
    for (size_t row = 0; row < 22; row++) {
        for (size_t col = 0; col < 10; col++) {
            gcx.drawTexture(TexID::MATRIXBG, {
                x + col * Mino::texture_size_px,
                y + row * Mino::texture_size_px,
                Mino::texture_size_px,
                Mino::texture_size_px
            });
        }
    }

    // Draw Minos
    for (size_t row = 0; row < 22; row++) {
        for (size_t col = 0; col < 10; col++) {
            if (matrix[row][col])
                matrix[row][col]->draw(gcx,
                                       x + col * Mino::texture_size_px,
                                       y + row * Mino::texture_size_px);
        }
    }
}

bool GameBoard::hasCollisionAt(int offset_x, unsigned offset_y)
{
    // At least one line of the piece grid must be on the board.
    // Horizontally, a piece can go between -3 and width+3,
    // vertically from 0 to heigh+3 (it cannot be over the board)
    assert(offset_x + 3 >= 0 && offset_x < matrix[0].size());
    assert(offset_y < matrix.size());

    if (!active_piece)
        return false;

    const size_t last_row = std::min<size_t>(offset_y + 3, matrix.size() - 1);
    const size_t first_col = std::max(0, offset_x);
    const size_t last_col = std::min<size_t>(offset_x, matrix[0].size() - 1);

    size_t piece_gridx = 0, piece_gridy = 0;
    for (size_t row = offset_y; row < last_row; row++) {
        for (size_t cell = first_col; cell < last_col; cell++) {
            bool board_has_mino_here = matrix.at(row).at(cell).operator bool();
            bool piece_has_mino_here = active_piece->currentGrid()
                                       .at(piece_gridy).at(piece_gridx).operator bool();
            if (piece_has_mino_here && board_has_mino_here)
                return true;
        }
        piece_gridy++;
        piece_gridx = 0;
    }

    return false;
}

void GameBoard::calculateGhostOffset()
{
    assert(active_piece);

    ghost_piece_y = active_piece_y;
    while (!hasCollisionAt(active_piece_x, ghost_piece_y) && ghost_piece_y < matrix.size())
        ghost_piece_y++;
}
