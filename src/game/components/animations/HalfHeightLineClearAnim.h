#pragma once

#include "LineClearAnim.h"


class HalfHeightLineClearAnim : public LineClearAnim {
public:
    HalfHeightLineClearAnim();
    void draw(GraphicsContext& gcx, int x, int y) const override;
};
