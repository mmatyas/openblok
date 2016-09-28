#pragma once

#include "WellAnimation.h"
#include "game/Transition.h"


class LineClearAnim : public WellAnimation {
public:
    LineClearAnim(unsigned row);

    void update(Duration t) final;
    void draw(GraphicsContext& gcx, int x, int y) const final;

    bool isActive() const final { return alpha.running(); }

private:
    const unsigned row;
    Transition<uint8_t> alpha;
};
