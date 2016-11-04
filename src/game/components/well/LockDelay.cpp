#include "LockDelay.h"

#include "game/components/Well.h"


namespace WellComponents {

LockDelay::LockDelay(Well& well, Duration delay, LockDelayType type, bool instant_harddrop)
    : harddrop_locks_instantly(instant_harddrop)
    , type(type)
    , reset_counter(reset_counter_max)
    , current_lowest_row(0)
    , countdown(delay, [](double){}, [&well](){
            well.lockThenRequestNext();
        })
{
}

void LockDelay::update(Well& well)
{
    if (well.isOnGround()) {
        countdown.unpause();
        // the piece should lock instantly on connect, if there are no more retries
        if (reset_counter == 0) {
            assert(type == LockDelayType::EXTENDED);
            countdown.update(countdown.length());
        }
    }
    else
        countdown.stop();
    countdown.update(Timing::frame_duration);
}

void LockDelay::cancel()
{
    reset_counter = reset_counter_max;
    current_lowest_row = 0;
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

void LockDelay::onDescend(Well& well)
{
    if (well.active_piece_y > current_lowest_row) {
        reset_counter = reset_counter_max;
        current_lowest_row = well.active_piece_y;
    }
}

void LockDelay::onHorizontalMove()
{
    switch (type) {
        case LockDelayType::CLASSIC:
            return;
        case LockDelayType::EXTENDED:
            if (reset_counter == 0)
                return;
            reset_counter--;
            // do NOT break
        case LockDelayType::INFINITE:
            countdown.stop();
            break;
    }
}

void LockDelay::onSuccesfulRotation()
{
    // same
    onHorizontalMove();
}

} // namespace WellComponents
