#pragma once

#include "ScoreTable.h"


struct PlayerStatistics {
    unsigned score;
    unsigned char level;
    unsigned short total_cleared_lines;
    unsigned short back_to_back_count;
    unsigned short back_to_back_longest;
    std::map<ScoreType, unsigned short> event_count;
};
