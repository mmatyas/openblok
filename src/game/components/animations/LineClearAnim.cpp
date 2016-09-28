#include "LineClearAnim.h"

#include "game/components/Mino.h"
#include "system/GraphicsContext.h"


LineClearAnim::LineClearAnim(unsigned row)
    : WellAnimation()
    , row(row)
    , alpha(frame_duration_60Hz * 40, [](double t){
            return static_cast<uint8_t>((1.0 - t) * 0xFF);
        })
{}

void LineClearAnim::update(Duration t)
{
    alpha.update(t);
}

void LineClearAnim::draw(GraphicsContext& gcx, int x, int y) const
{
    // TODO: fix magic numbers
    gcx.drawFilledRect({x, static_cast<int>(y + (row - 2) * Mino::texture_size_px),
                        Mino::texture_size_px * 10, Mino::texture_size_px},
                       {0xFF, 0xFF, 0xFF, alpha.value()});
}
