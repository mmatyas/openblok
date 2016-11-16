#pragma once

#include "game/Timing.h"
#include "game/components/NextQueue.h"
#include "game/layout/Box.h"
#include "system/Color.h"

#include <string>


class AppContext;
class Font;
class Texture;

namespace Layout {
class RightSidebarBox : Layout::Box {
public:
    RightSidebarBox(AppContext&, int height);

    void setPosition(int x, int y) override;

    void update();
    void updateScore(unsigned);
    void updateGametime(Duration);

    void draw(GraphicsContext&) const;

    NextQueue& nextQueue() { return next_queue; }

private:
    static constexpr int text_height = 30;
    static constexpr int text_padding = 10;
    static constexpr int item_padding = 10;
    const RGBAColor box_color = 0x0A0AFF80_rgba;

    std::shared_ptr<Font> font_label;
    std::shared_ptr<Font> font_content;

    std::unique_ptr<Texture> tex_next;
    NextQueue next_queue;

    ::Rectangle rect_score;
    std::unique_ptr<Texture> tex_score;
    std::unique_ptr<Texture> tex_score_counter;

    std::string gametime_text;
    ::Rectangle rect_time;
    std::unique_ptr<Texture> tex_time_counter;
};
} // namespace Layout
