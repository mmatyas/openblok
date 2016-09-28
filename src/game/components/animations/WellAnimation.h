#pragma once

#include "game/Timing.h"


class GraphicsContext;

class WellAnimation {
public:
    virtual ~WellAnimation() {}

    virtual void update(Duration t) = 0;
    virtual bool isActive() const = 0;
    virtual void draw(GraphicsContext&, int x, int y) const = 0;
};
