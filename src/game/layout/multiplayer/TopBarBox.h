#pragma once

#include "game/components/HoldQueue.h"
#include "game/components/NextQueue.h"
#include "game/layout/Box.h"
#include "system/Color.h"


class AppContext;
class Texture;

namespace Layout {
class TopBarBox : public Layout::Box {
public:
    TopBarBox(AppContext&);

    void update();
    void draw(GraphicsContext&) const;

    HoldQueue& holdQueue() { return hold_queue; }
    NextQueue& nextQueue() { return next_queue; }

private:
    static constexpr int text_height = 30;
    static constexpr int text_padding = 10;
    static constexpr int item_padding = 10;
    const RGBAColor box_color = 0x0A0AFF80_rgba;

    std::unique_ptr<Texture> tex_hold;
    HoldQueue hold_queue;

    std::unique_ptr<Texture> tex_next;
    NextQueue next_queue;
};
} // namespace Layout
