#pragma once

#include "GarbageGauge.h"
#include "PlayerArea.h"
#include "narrow/BottomBarBox.h"
#include "narrow/TopBarBox.h"


namespace Layout {
class NarrowBattlePA : public Layout::PlayerArea {
public:
    NarrowBattlePA(AppContext&);

    void update() override;
    void setPosition(int x, int y) override;
    void drawActive(GraphicsContext&) const override;
    void drawPassive(GraphicsContext&) const override;

    void updateLevelCounter(unsigned) override;
    void updateScore(unsigned) override;
    void updateGarbageGauge(unsigned) override;

    unsigned queuedGarbageLines() const override { return ui_gauge.lineCount(); }
    HoldQueue& holdQueue() override { return ui_top.holdQueue(); }
    NextQueue& nextQueue() override { return ui_top.nextQueue(); }

private:
    static constexpr int inner_padding = 10;
    GarbageGauge ui_gauge;
    TopBarBox ui_top;
    BottomBarBox ui_bottom;
};
} // namespace Layout


