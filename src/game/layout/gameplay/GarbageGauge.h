#pragma once

#include "game/layout/Box.h"

class AppContext;
class GraphicsContext;


namespace Layout {
class GarbageGauge : public Layout::Box {
public:
    GarbageGauge(AppContext&, int height);

    void setPosition(int x, int y) override;
    void drawPassive(GraphicsContext&) const;
    void drawActive(GraphicsContext&) const;

    void setLineCount(unsigned short);
    unsigned short lineCount() const { return line_count; }

private:
    unsigned short line_count;
    const int cell_height;
    static constexpr int GAUGE_WIDTH = 10;
    static constexpr int BORDER_WIDTH = 5;
    static constexpr unsigned short MAX_LINES = 20;

    ::Rectangle border_right;
    ::Rectangle border_top;
    ::Rectangle border_bottom;
};
} // namespace Layout
