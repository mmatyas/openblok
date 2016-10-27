#pragma once

#include "game/Timing.h"


class Well;


namespace WellComponents {

class AutoRepeat  {
public:
    AutoRepeat(Duration shift_normal = Duration::max(), Duration shift_turbo = Duration::max());

    /// turn off autorepeat mode
    void reset();

    void update();
    bool movementAllowed();
    void onHorizontalMove();

private:
    const Duration horizontal_delay_normal; // normal key repeat rate
    const Duration horizontal_delay_turbo; // turbo key repeat rate
    Duration horizontal_delay_current; // current input rate, either normal or turbo
    Duration horizontal_timer; // timer for horizontal autorepeat
    Duration das_timer; // timer for turbo mode activation
};

} // namespace WellComponents
