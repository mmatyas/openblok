#pragma once

#include "game/Timing.h"


class Well;


namespace WellComponents {

class AutoRepeat  {
public:
    AutoRepeat(Duration shift_normal = Duration::max(), Duration shift_turbo = Duration::max());

    /// turn off autorepeat mode
    void reset();
    /// update the autorepeat timer
    void update();

    bool movementAllowed();
    void onSimpleMove();
    void onDASMove();

private:
    const Duration time_to_activate;
    const Duration autorepeat_delay;
    Duration das_timer; // timer for turbo mode activation
};

} // namespace WellComponents
