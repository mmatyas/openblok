#include "PlayerArea.h"

namespace Layout {

PlayerArea::PlayerArea(AppContext& app)
    : ui_well(app)
{
}

void PlayerArea::calcWellBox()
{
    wellbox = {
        ui_well.wellX(), ui_well.wellY(),
        ui_well.wellWidth(), ui_well.wellHeight()
    };
}

} // namespace Layout
