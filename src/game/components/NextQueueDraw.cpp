#include "NextQueueDraw.h"

#include "Mino.h"
#include "NextQueue.h"
#include "PieceDraw.h"
#include "system/GraphicsContext.h"

#include <assert.h>


namespace {

void draw_nth_piece(const NextQueue& queue, unsigned i, int x, int y)
{
    assert(i < queue.piece_queue.size());
    assert(i < queue.displayed_piece_count);

    const auto& piece = queue.m_piece_storage.at(static_cast<size_t>(queue.m_piece_queue.at(i)));
    const float padding_x = (4 - ::displayWidth(piece->type())) / 2.0f;
    ::drawPiece(*piece, x + Mino::texture_size_px * (0.5f + padding_x), y);
}

} // namespace

void NextQueueDraw::draw(const NextQueue& queue, GraphicsContext& gcx, int x, int y)
{
    if (!queue.previewCount())
        return;

    int offset_y = y + Mino::texture_size_px;
    draw_nth_piece(queue, 0, x, offset_y);
    offset_y += Mino::texture_size_px * 3;

    const auto scale = gcx.getDrawScale();
    gcx.modifyDrawScale(scale * 0.75);
    x *= (1 / 0.75);
    offset_y *= (1 / 0.75);
    offset_y += Mino::texture_size_px;
    for (unsigned i = 1; i < queue.previewCount(); i++) {
        draw_nth_piece(queue, i, x, offset_y);
        offset_y += Mino::texture_size_px * 3;
    }
    gcx.modifyDrawScale(scale);
}
