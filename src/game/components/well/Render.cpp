#include "Render.h"

#include "game/components/Mino.h"
#include "game/components/MinoStorage.h"
#include "game/components/Piece.h"
#include "game/components/Well.h"
#include "game/components/animations/WellAnimation.h"

#include <stddef.h>


namespace WellComponents {

void Render::drawBackground(const Well&, GraphicsContext&, int draw_offset_x, int draw_offset_y) const
{
    const auto& cell = MinoStorage::getMatrixCell();
    for (size_t row = 0; row < 20; row++) {
        for (size_t col = 0; col < 10; col++) {
            cell->draw(draw_offset_x + col * Mino::texture_size_px,
                       draw_offset_y + row * Mino::texture_size_px);
        }
    }
}

void Render::drawContent(const Well& well, GraphicsContext& gcx, int draw_offset_x, int draw_offset_y) const
{
    // Draw board Minos
    for (size_t row = 0; row < 20; row++) {
        for (size_t col = 0; col < 10; col++) {
            const auto& cell = well.matrix.at(row + 2).at(col);
            if (cell) {
                cell->draw(draw_offset_x + col * Mino::texture_size_px,
                           draw_offset_y + row * Mino::texture_size_px);
            }
        }
    }

    // Draw current piece
    if (well.active_piece) {
        // draw ghost
        for (unsigned row = 0; row < 4; row++) {
            if (well.ghost_piece_y + row < 2) // hide rows 21-22
                continue;
            for (unsigned col = 0; col < 4; col++) {
                if (well.active_piece->currentGrid().at(row).at(col)) {
                    const auto& cell = MinoStorage::getGhost(well.active_piece->type());
                    cell->draw(draw_offset_x + (well.active_piece_x + col) * Mino::texture_size_px,
                               draw_offset_y + (well.ghost_piece_y + row - 2) * Mino::texture_size_px);
                }
            }
        }

        // draw piece
        for (unsigned row = 0; row < 4; row++) {
            if (well.active_piece_y + row < 2) // hide rows 21-22
                continue;
            for (unsigned col = 0; col < 4; col++) {
                const auto& cell = well.active_piece->currentGrid().at(row).at(col);
                if (cell) {
                    cell->draw(draw_offset_x + (well.active_piece_x + col) * Mino::texture_size_px,
                               draw_offset_y + (well.active_piece_y + row - 2) * Mino::texture_size_px);
                }
            }
        }
    }

    // Draw animations
    for (auto& anim : well.animations)
        anim->draw(gcx, draw_offset_x, draw_offset_y);
}

} // namespace WellComponents
