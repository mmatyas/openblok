#pragma once

#include "system/Rectangle.h"


class GraphicsContext;
class Well;


namespace WellComponents {

class Render {
public:
    Render();
    void drawContent(const Well&, GraphicsContext&, int draw_offset_x, int draw_offset_y) const;

private:
    const int top_row_height;
    const Rectangle top_row_cliprect;
};

} // namespace WellComponents

