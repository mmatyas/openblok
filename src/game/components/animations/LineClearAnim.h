#pragma once

#include "WellAnimation.h"
#include "game/Transition.h"
#include "system/Color.h"


class LineClearAnim : public WellAnimation {
public:
    LineClearAnim(unsigned row);
    virtual ~LineClearAnim() {}

    void update(Duration t) override;
    void draw(GraphicsContext& gcx, int x, int y) const override;

    bool isActive() const final { return row_percent.running(); }

    static RGBAColor anim_color;

protected:
    const int row;
    Transition<double> row_percent;
};
