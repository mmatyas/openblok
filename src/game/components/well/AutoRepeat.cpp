#include "AutoRepeat.h"



namespace WellComponents {

AutoRepeat::AutoRepeat(Duration shift_normal, Duration shift_turbo)
    : horizontal_delay_normal(shift_normal)
    , horizontal_delay_turbo(shift_turbo)
    , horizontal_delay_current(horizontal_delay_normal)
    , horizontal_timer(Duration::zero())
    , das_timer(horizontal_delay_normal)
{
}

void AutoRepeat::reset()
{
    das_timer = horizontal_delay_normal;
    horizontal_delay_current = horizontal_delay_normal;
}

void AutoRepeat::update()
{
    horizontal_timer -= Timing::frame_duration;
}

bool AutoRepeat::movementAllowed()
{
    return horizontal_timer <= Duration::zero();
}

void AutoRepeat::onHorizontalMove()
{
    das_timer -= horizontal_delay_normal;
    if (das_timer < Duration::zero())
        horizontal_delay_current = horizontal_delay_turbo;

    horizontal_timer = horizontal_delay_current;
}


} // namespace WellComponents
