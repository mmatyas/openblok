#pragma once

#include "Box.h"
#include "game/components/Well.h"

#include <vector>


class AppContext;
class Font;
class Texture;

namespace Layout {
class WellBox : public Layout::Box {
public:
    WellBox(AppContext&);

    void setPosition(int x, int y) override;
    void update(const std::vector<InputEvent>&, AppContext&);
    void draw(GraphicsContext&, bool paused) const;

    Well& well() { return m_well; }

private:
    Well m_well;

    std::shared_ptr<Font> font_big;
    std::unique_ptr<Texture> tex_pause;

    static constexpr uint8_t border_width = 5;
    ::Rectangle border_left;
    ::Rectangle border_right;
    ::Rectangle border_top;
    ::Rectangle border_bottom;
};
} // namespace Layout
