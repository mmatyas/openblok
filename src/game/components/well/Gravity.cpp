#include "Gravity.h"

#include "game/components/Well.h"


namespace WellComponents {

Gravity::Gravity(Duration delay)
    : gravity_delay(delay)
    , gravity_timer(Duration::zero())
    , skip_gravity(false)
{
}

void Gravity::skipNextUpdate()
{
    skip_gravity = true;
}

void Gravity::update(Well& well)
{
    gravity_timer += Timing::frame_duration;
    while (gravity_timer >= gravity_delay) {
        gravity_timer -= gravity_delay;

        // do not apply downward movement twice
        if (!skip_gravity)
            applyGravity(well);
    }

    skip_gravity = false;
}

void Gravity::applyGravity(Well& well)
{
    well.moveDownNow();
}

} // namespace WellComponents
