#include "Render.h"

#include "game/components/Mino.h"
#include "game/components/MinoStorage.h"
#include "game/components/Piece.h"
#include "game/components/Well.h"
#include "game/components/animations/WellAnimation.h"

#include <stddef.h>


namespace WellComponents {

Render::Render()
    : top_row_height(Mino::texture_size_px * 0.3)
    , top_row_cliprect({0, Mino::texture_size_px - top_row_height, Mino::texture_size_px, top_row_height})
{}

void Render::drawContent(const Well& well, GraphicsContext& gcx, int draw_offset_x, int draw_offset_y) const
{
    // TODO: this could be optimized

    // Draw board Minos
    for (int col = 0; col < 10; col++) {
        const char type_char = well.matrix.at(19).at(col);
        if (type_char) {
            const auto& mino = MinoStorage::getMino(::typeFromAscii(type_char));
            mino->drawPartial(top_row_cliprect, {
                draw_offset_x + col * Mino::texture_size_px, draw_offset_y,
                Mino::texture_size_px, top_row_height});
        }
    }
    draw_offset_y += top_row_height;
    for (unsigned row = 0; row < 20; row++) {
        for (unsigned col = 0; col < 10; col++) {
            const char type_char = well.matrix.at(row + 20).at(col);
            if (type_char) {
                const auto& mino = MinoStorage::getMino(::typeFromAscii(type_char));
                mino->draw(draw_offset_x + col * Mino::texture_size_px,
                           draw_offset_y + row * Mino::texture_size_px);
            }
        }
    }

    // Draw current piece
    if (well.active_piece) {
        // draw ghost
        const auto& ghost_cell = MinoStorage::getGhost(well.active_piece->type());
        for (unsigned row = 0; row < 4; row++) {
            if (well.ghost_piece_y + row < 20) // hide buffer zone
                continue;
            for (unsigned col = 0; col < 4; col++) {
                if (well.active_piece->currentGrid().at(row).at(col)) {
                    ghost_cell->draw(draw_offset_x + (well.active_piece_x + col) * Mino::texture_size_px,
                                     draw_offset_y + (well.ghost_piece_y + row - 20) * Mino::texture_size_px);
                }
            }
        }

        // draw piece
        for (int row = 0; row < 4; row++) {
            if (well.active_piece_y + row < 19) // hide buffer zone
                continue;

            if (well.active_piece_y + row < 20) { // partially draw the topmost row
                draw_offset_y -= top_row_height;
                for (int col = 0; col < 4; col++) {
                    const bool has_mino = well.active_piece->currentGrid().at(row).at(col);
                    if (has_mino) {
                        const auto& mino = MinoStorage::getMino(well.active_piece->type());
                        mino->drawPartial(top_row_cliprect, {
                            draw_offset_x + (well.active_piece_x + col) * Mino::texture_size_px,
                            draw_offset_y + (well.active_piece_y + row - 19) * Mino::texture_size_px,
                            Mino::texture_size_px, top_row_height});
                    }
                }
                draw_offset_y += top_row_height;
                continue;
            }

            for (unsigned col = 0; col < 4; col++) {
                const bool has_mino = well.active_piece->currentGrid().at(row).at(col);
                if (has_mino) {
                    const auto& mino = MinoStorage::getMino(well.active_piece->type());
                    mino->draw(draw_offset_x + (well.active_piece_x + col) * Mino::texture_size_px,
                               draw_offset_y + (well.active_piece_y + row - 20) * Mino::texture_size_px);
                }
            }
        }
    }

    // Draw animations
    for (auto& anim : well.animations)
        anim->draw(gcx, draw_offset_x, draw_offset_y);
}

} // namespace WellComponents
