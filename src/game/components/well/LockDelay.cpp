#include "LockDelay.h"

#include "game/components/Well.h"


namespace WellComponents {

LockDelay::LockDelay(Well& well, Duration lock_delay, bool infinity_lock, bool instant_harddrop)
    : harddrop_locks_instantly(instant_harddrop)
    , infinity_lock(infinity_lock)
    , countdown(lock_delay, [](double){}, [&well](){
            well.lockThenRequestNext();
        })
{
}

void LockDelay::update(Well& well)
{
    if (well.isOnGround())
        countdown.unpause();
    else
        countdown.stop();
    countdown.update(Timing::frame_duration);
}

void LockDelay::cancel()
{
    countdown.stop();
}

bool LockDelay::sonicLockPossible() const
{
    return !harddropLocksInstantly() && lockInProgress();
}

bool LockDelay::lockInProgress() const
{
    return countdown.running();
}

void LockDelay::onHorizontalMove()
{
    if (infinity_lock)
        countdown.stop();
}

void LockDelay::onSuccesfulRotation()
{
    onHorizontalMove();
}

} // namespace WellComponents
