#include "Gravity.h"

#include "game/components/Well.h"


constexpr Duration GRAVITY_20G = Timing::frame_duration_60Hz / 20;


namespace WellComponents {

Gravity::Gravity(Duration duration)
    : gravity_delay(duration)
    , gravity_timer(Duration::zero())
    , skip_gravity(false)
{
}

void Gravity::setRate(Duration duration)
{
    gravity_delay = std::max<Duration>(duration, GRAVITY_20G);
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
