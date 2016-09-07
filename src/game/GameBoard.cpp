#include "GameBoard.h"

#include "PieceFactory.h"
#include "Resources.h"
#include "system/GraphicsContext.h"

#include <assert.h>


GameBoard::GameBoard()
    : active_piece_x(0)
    , active_piece_y(0)
{
}

void GameBoard::addPiece(Piece::Type type)
{
    // the player can only control one piece at a time
    assert(!active_piece);

    active_piece = PieceFactory::make_uptr(type);
    active_piece_x = 3;
    active_piece_y = 0;
}

std::string GameBoard::asAscii()
{
    // the piece must be inside the grid
    assert(active_piece_x + 4 < matrix[0].size());
    assert(active_piece_y + 4 < matrix.size());

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

