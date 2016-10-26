#pragma once

#include "game/Transition.h"


class Well;


namespace WellComponents {

class LockDelay {
public:
    LockDelay(Well&, Duration lock_delay, bool infinity_lock = true, bool instant_harddrop = true);

    void update(Well&);
    void cancel();

    bool harddropLocksInstantly() const { return harddrop_locks_instantly; }
    /// Sonic locking is possible if hard drop doesn't lock instantly (Sonic Drop),
    /// and the piece has started locking, but not finished yet
    bool sonicLockPossible() const;
    /// The piece has started locking, but not finished yet
    bool lockInProgress() const;

    void onHorizontalMove();
    void onSuccesfulRotation();

private:
    const bool harddrop_locks_instantly;
    const bool infinity_lock;

    Transition<void> countdown; //< starts when a piece reaches the ground, resets on movement
};

} // namespace WellComponents
