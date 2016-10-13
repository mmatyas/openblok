#pragma once

#include "Box.h"
#include "game/Timing.h"
#include "game/components/NextQueue.h"


class AppContext;

namespace Layout {
class RightSidebarBox : Layout::Box {
public:
    RightSidebarBox(AppContext&, uint16_t height);

    void setPosition(uint16_t x, uint16_t y) override;

    void update();
    void updateScore(GraphicsContext&, unsigned);
    void updateGametime(GraphicsContext&);

    void draw(GraphicsContext&) const;

    NextQueue& nextQueue() { return next_queue; }

private:
    static constexpr int text_height = 30;
    static constexpr int text_padding = 10;
    static constexpr int item_padding = 10;
    const RGBAColor box_color = 0x0A0AFF80_rgba;

    FontID font_label;
    FontID font_content;

    TextureID tex_next;
    NextQueue next_queue;

    ::Rectangle rect_score;
    TextureID tex_score;
    TextureID tex_score_counter;

    Duration gametime;
    std::string gametime_text;
    ::Rectangle rect_time;
    TextureID tex_time_counter;
};
} // namespace Layout
