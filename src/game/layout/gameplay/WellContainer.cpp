#include "WellContainer.h"

#include "game/AppContext.h"
#include "game/components/animations/LineClearAnim.h"
#include "game/components/rotations/RotationFactory.h"
#include "system/GraphicsContext.h"


namespace Layout {

WellContainer::WellContainer(AppContext& app)
    : m_well(app.wellconfig())
{
    LineClearAnim::anim_color = app.theme().colors.line_clear;

    bounding_box.w = wellWidth() + border_width * 2;
    bounding_box.h = wellHeight() + border_width * 2;

    setPosition(0, 0);
}

void WellContainer::setPosition(int x, int y)
{
    bounding_box.x = x;
    bounding_box.y = y;
}

void WellContainer::drawContent(GraphicsContext& gcx) const
{
    m_well.drawContent(gcx, x() + border_width, y() + border_width);
}

} // namespace Layout

