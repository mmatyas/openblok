#include "HalfHeightLineClearAnim.h"

#include "game/components/Mino.h"
#include "system/GraphicsContext.h"


HalfHeightLineClearAnim::HalfHeightLineClearAnim()
    : LineClearAnim(19)
{}

void HalfHeightLineClearAnim::draw(GraphicsContext& gcx, int x, int y) const
{
    // TODO: fix magic numbers
    static const int row_width = Mino::texture_size_px * 10;
    gcx.drawFilledRect({
        x + static_cast<int>(row_percent.value() * row_width),
        y,
        static_cast<int>(row_width * (1 - row_percent.value())),
        static_cast<int>(Mino::texture_size_px * 0.3f)},
        0xEEEEEE_rgb);
}
