#pragma once

class GraphicsContext;
class NextQueue;


class NextQueueDraw {
public:
    /// Draw the N previewable pieces at (x,y)
    static void draw(const NextQueue&, GraphicsContext&, int x, int y);
};
