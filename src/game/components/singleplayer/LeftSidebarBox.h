#pragma once

#include "Box.h"
#include "game/components/HoldQueue.h"


class AppContext;

namespace Layout {
class LeftSidebarBox : public Layout::Box {
public:
    LeftSidebarBox(AppContext&, uint16_t height);

    void setPosition(uint16_t x, uint16_t y) override;

    void update();
    void updateGoalCounter(GraphicsContext&, unsigned);
    void updateLevelCounter(GraphicsContext&, unsigned);

    void draw(GraphicsContext&) const;

    HoldQueue& holdQueue() { return hold_queue; }

private:
    static constexpr int text_height = 30;
    static constexpr int text_padding = 10;
    static constexpr int item_padding = 10;
    const RGBAColor box_color = 0x0A0AFF80_rgba;

    FontID font_label;
    FontID font_content;

    TextureID tex_hold;
    HoldQueue hold_queue;

    ::Rectangle rect_goal;
    TextureID tex_goal;
    TextureID tex_goal_counter;

    ::Rectangle rect_level;
    TextureID tex_level;
    TextureID tex_level_counter;
};
} // namespace Layout
