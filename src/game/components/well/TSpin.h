#pragma once

#include <stdint.h>


enum class TSpinDetectionResult : uint8_t {
    NONE,
    TSPIN,
    MINI_TSPIN
};


class Well;


namespace WellComponents {

class TSpin {
public:
    TSpin(bool enabled = true, bool allow_wall = true, bool allow_kick = true);

    TSpinDetectionResult check(Well&);

    void clear();
    void onWallKick();
    void onSuccesfulRotation();

private:
    const bool enabled;
    const bool allow_wall;
    const bool allow_kick;

    bool allowed;
    // SRS defines multiple rotation points, a proper tspin requires the use of the last one (== 3)
    uint8_t last_rotation_point;
};

} // namespace WellComponents
