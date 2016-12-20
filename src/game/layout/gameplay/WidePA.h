#pragma once

#include "PlayerArea.h"
#include "wide/LeftSidebarBox.h"
#include "wide/RightSidebarBox.h"


namespace Layout {
class WidePA : public Layout::PlayerArea {
public:
    WidePA(AppContext&);

    void update() override;
    void setPosition(int x, int y) override;
    void drawActive(GraphicsContext&) const override;
    void drawPassive(GraphicsContext&) const override;

    void updateLevelCounter(unsigned) override;
    void updateScore(unsigned) override;
    void updateGametime(Duration) override;
    void updateGoalCounter(unsigned) override;

    HoldQueue& holdQueue() override { return ui_left.holdQueue(); }
    NextQueue& nextQueue() override { return ui_right.nextQueue(); }

private:
    static constexpr int inner_padding = 10;
    LeftSidebarBox ui_left;
    RightSidebarBox ui_right;
};
} // namespace Layout
