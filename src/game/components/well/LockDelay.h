#pragma once

#include "game/Transition.h"
#include "game/components/LockDelayType.h"


class Well;


namespace WellComponents {

class LockDelay {
public:
    LockDelay(Well&, Duration delay, LockDelayType type, bool instant_harddrop = true);

    void update(Well&);
    void cancel();

    bool harddropLocksInstantly() const { return harddrop_locks_instantly; }
    /// Sonic locking is possible if hard drop doesn't lock instantly (Sonic Drop),
    /// and the piece has started locking, but not finished yet
    bool sonicLockPossible() const;
    /// The piece has started locking, but not finished yet
    bool lockInProgress() const;

    void onDescend(Well&);
    void onHorizontalMove();
    void onSuccesfulRotation();

private:
    const bool harddrop_locks_instantly;
    const LockDelayType type;
    static constexpr uint8_t reset_counter_max = 10;
    uint8_t reset_counter;
    uint8_t current_lowest_row; ///< this number grows the deeper you are in the well

    Transition<void> countdown; ///< starts when a piece reaches the ground, resets on movement
};

} // namespace WellComponents
