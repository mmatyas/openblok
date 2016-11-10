#include "AutoRepeat.h"


namespace WellComponents {

AutoRepeat::AutoRepeat(Duration shift_normal, Duration shift_turbo)
    : time_to_activate(shift_normal)
    , autorepeat_delay(shift_turbo)
    , das_timer(Duration::max())
{
}

void AutoRepeat::reset()
{
    das_timer = Duration::max();
}

void AutoRepeat::update()
{
    das_timer -= Timing::frame_duration;
}

bool AutoRepeat::movementAllowed()
{
    return das_timer <= Duration::zero();
}

bool AutoRepeat::inactive()
{
    return das_timer > time_to_activate;
}

void AutoRepeat::activate()
{
    das_timer = time_to_activate;
}

void AutoRepeat::onDASMove()
{
    das_timer = autorepeat_delay;
}

} // namespace WellComponents
