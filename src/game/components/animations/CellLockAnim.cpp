#include "CellLockAnim.h"

#include "game/components/Mino.h"
#include "game/Timing.h"


CellLockAnim::CellLockAnim(unsigned row, unsigned col)
    : WellAnimation()
    , cell_x(col * Mino::texture_size_px)
    , cell_y_top(row * Mino::texture_size_px)
    , cell_y_bottom(cell_y_top + Mino::texture_size_px)
    , anim_y_top(frame_duration_60Hz * 20, [this](double t){
        return this->cell_y_bottom - t * Mino::texture_size_px * 2;
    })
{}

void CellLockAnim::update(Duration t) {
    anim_y_top.update(t);
}

void CellLockAnim::draw(GraphicsContext& gcx, int offset_x, int offset_y) const {
    static const int effect_height_max = Mino::texture_size_px / 2;
    int y_top = std::max<int>(anim_y_top.value(), cell_y_top);
    int y_bottom = std::min<int>(anim_y_top.value() + effect_height_max, cell_y_bottom);
    if (y_bottom <= y_top)
        return;

    gcx.drawFilledRect({
        static_cast<int>(offset_x + cell_x), offset_y + y_top,
        Mino::texture_size_px, y_bottom - y_top},
        0xFFFFFF30_rgba);
}
