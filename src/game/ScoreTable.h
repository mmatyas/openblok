#pragma once

#include "WellEvent.h"

#include <map>
#include <string>


enum class ScoreType : unsigned char {
    CLEAR_SINGLE,
    CLEAR_DOUBLE,
    CLEAR_TRIPLE,
    CLEAR_PERFECT,
    MINI_TSPIN,
    CLEAR_MINI_TSPIN_SINGLE,
    TSPIN,
    CLEAR_TSPIN_SINGLE,
    CLEAR_TSPIN_DOUBLE,
    CLEAR_TSPIN_TRIPLE,
    SOFTDROP,
    HARDDROP,
    COMBO,
};


class ScoreTable {
public:
    static unsigned short value(ScoreType type) { return score_table.at(type); }
    static const std::string& name(ScoreType type) { return score_name.at(type); }

    static ScoreType lineclearType(const WellEvent::lineclear_t&);
    static unsigned short lineAwards(ScoreType type) { return lineaward_table.at(type); }

    static bool canContinueBackToBack(ScoreType previous, ScoreType current);
    static float back2backMultiplier() { return back2back_multiplier; }
    static const std::string back2backName() { return back2back_name; }

private:
    static const std::map<ScoreType, unsigned short> score_table;
    static const std::map<ScoreType, const std::string> score_name;
    static const std::map<ScoreType, unsigned short> lineaward_table;

    static float back2back_multiplier;
    static const std::string back2back_name;
};
