#include "LineClearAnim.h"

#include "game/components/Mino.h"
#include "system/GraphicsContext.h"


constexpr Duration TIME_PER_ROW = Timing::frame_duration_60Hz * 40;

RGBAColor LineClearAnim::anim_color = 0xEEEEEEFF_rgba;

LineClearAnim::LineClearAnim(unsigned row)
    : WellAnimation()
    , row(row)
    , row_percent(TIME_PER_ROW, [this](double t){
            return t;
        })
{}

void LineClearAnim::update(Duration t)
{
    row_percent.update(t);
}

void LineClearAnim::draw(GraphicsContext& gcx, int x, int y) const
{
    // TODO: fix magic numbers
    static const int row_width = Mino::texture_size_px * 10;
    gcx.drawFilledRect({
        x + static_cast<int>(row_percent.value() * row_width),
        y + (row - 20) * Mino::texture_size_px,
        static_cast<int>(row_width * (1 - row_percent.value())),
        Mino::texture_size_px},
        anim_color);
}
