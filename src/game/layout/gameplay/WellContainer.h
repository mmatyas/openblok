#pragma once

#include "game/components/Mino.h"
#include "game/components/Well.h"
#include "game/layout/Box.h"

class AppContext;


namespace Layout {
class WellContainer : public Layout::Box {
public:
    WellContainer(AppContext&);

    void setPosition(int x, int y) override;
    void drawBase(GraphicsContext&) const;
    void drawContent(GraphicsContext&) const;

    Well& well() { return m_well; }

    int wellWidth() const { return 10 * Mino::texture_size_px; }
    int wellHeight() const { return 20.3 * Mino::texture_size_px; }
    int wellX() const { return x() + border_width; }
    int wellY() const { return y() + border_width; }

private:
    Well m_well;

    static constexpr uint8_t border_width = 5;
    ::Rectangle border_left;
    ::Rectangle border_right;
    ::Rectangle border_top;
    ::Rectangle border_bottom;
};
} // namespace Layout
