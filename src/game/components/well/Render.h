#pragma once

class GraphicsContext;
class Well;


namespace WellComponents {

class Render {
public:
    void drawBackground(const Well&, GraphicsContext&, int draw_offset_x, int draw_offset_y) const;
    void drawContent(const Well&, GraphicsContext&, int draw_offset_x, int draw_offset_y) const;
};

} // namespace WellComponents

