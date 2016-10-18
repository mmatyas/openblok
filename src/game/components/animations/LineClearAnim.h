#pragma once

#include "WellAnimation.h"
#include "game/Transition.h"


class LineClearAnim : public WellAnimation {
public:
    LineClearAnim(unsigned row);

    void update(Duration t) final;
    void draw(GraphicsContext& gcx, int x, int y) const final;

    bool isActive() const final { return row_percent.running(); }

private:
    const int row;
    Transition<double> row_percent;
};
