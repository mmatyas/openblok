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
    TopBarBox(AppContext&, int width);

    void update();
    void drawActive(GraphicsContext&) const;
    void drawPassive(GraphicsContext&) const;

    HoldQueue& holdQueue() { return hold_queue; }
    NextQueue& nextQueue() { return next_queue; }

private:
    std::unique_ptr<Texture> tex_hold;
    HoldQueue hold_queue;

    std::unique_ptr<Texture> tex_next;
    NextQueue next_queue;
};
} // namespace Layout
