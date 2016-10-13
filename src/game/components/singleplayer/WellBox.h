#pragma once

#include "game/components/Well.h"
#include "system/GraphicsContext.h"

#include <vector>


class AppContext;

namespace Layout {
class WellBox {
public:
    WellBox(AppContext&);

    void setPosition(uint16_t x, uint16_t y);
    void update(const std::vector<InputEvent>&, AppContext&);
    void draw(GraphicsContext&, bool paused) const;

    Well& well() { return m_well; }

    uint16_t x() const { return bounding_box.x; }
    uint16_t y() const { return bounding_box.y; }
    uint16_t width() const { return bounding_box.w; }
    uint16_t height() const { return bounding_box.h; }

private:
    ::Rectangle bounding_box;

    Well m_well;

    FontID font_big;
    TextureID tex_pause;

    static constexpr uint8_t border_width = 5;
    ::Rectangle border_left;
    ::Rectangle border_right;
    ::Rectangle border_top;
    ::Rectangle border_bottom;
};
} // namespace Layout
