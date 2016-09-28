#pragma once

#include "WellAnimation.h"
#include "game/Transition.h"


class CellLockAnim : public WellAnimation {
public:
    CellLockAnim(unsigned row, unsigned col);

    void update(Duration t) final;
    void draw(GraphicsContext& gcx, int x, int y) const final;

    bool isActive() const final { return anim_y_top.running(); }

private:
    const unsigned cell_x;
    const unsigned cell_y_top;
    const unsigned cell_y_bottom;

    Transition<int> anim_y_top;
};
