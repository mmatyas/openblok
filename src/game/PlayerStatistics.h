#pragma once

#include "ScoreTable.h"
#include "Timing.h"


struct PlayerStatistics {
    unsigned score;
    unsigned char level;
    unsigned short total_cleared_lines;
    unsigned short back_to_back_count;
    unsigned short back_to_back_longest;
    std::map<ScoreType, unsigned short> event_count;
    Duration gametime;

    PlayerStatistics()
        : score(0), level(1), total_cleared_lines(0)
        , back_to_back_count(0), back_to_back_longest(0)
        , gametime(Duration::zero())
    {}
};
