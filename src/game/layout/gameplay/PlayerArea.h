#pragma once

#include "WellContainer.h"
#include "game/Timing.h"
#include "game/layout/Box.h"
#include "game/components/Mino.h"

class AppContext;
class GraphicsContext;
class HoldQueue;
class NextQueue;
class Well;


namespace Layout {
class PlayerArea : public Layout::Box {
public:
    PlayerArea(AppContext& app);
    virtual ~PlayerArea() {}

    virtual void update() {}
    void setPosition(int x, int y) override = 0;
    virtual void drawActive(GraphicsContext&) const {}
    virtual void drawPassive(GraphicsContext&) const {}

    virtual void updateLevelCounter(unsigned) {}
    virtual void updateScore(unsigned) {}
    virtual void updateGametime(Duration) {}
    virtual void updateGoalCounter(unsigned) {}

    virtual HoldQueue& holdQueue() = 0;
    virtual NextQueue& nextQueue() = 0;

    virtual Well& well() { return ui_well.well(); };
    virtual ::Rectangle wellBox() const { return wellbox; }
    virtual int wellCenterX() const { return wellBox().x + wellBox().w / 2; }
    virtual int wellCenterY() const { return wellBox().y + wellBox().h / 2; }

protected:
    ::Rectangle wellbox;
    WellContainer ui_well;

    void calcWellBox();
};
} // namespace Layout
