#include "GameBoard.h"

#include "Resources.h"
#include "system/GraphicsContext.h"


GameBoard::GameBoard()
{
}

std::string GameBoard::asAscii()
{
    std::string output;
    for (const auto& row : matrix) {
        for (const auto& cell : row) {
            if (cell)
                output += cell->asAscii();
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

