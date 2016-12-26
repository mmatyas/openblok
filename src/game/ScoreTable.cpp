#include "ScoreTable.h"

#include "system/Localize.h"

#include <set>
#include <assert.h>


const std::map<ScoreType, unsigned short> ScoreTable::score_table = {
    {ScoreType::CLEAR_SINGLE, 100},
    {ScoreType::CLEAR_DOUBLE, 200},
    {ScoreType::CLEAR_TRIPLE, 500},
    {ScoreType::CLEAR_PERFECT, 800},
    {ScoreType::MINI_TSPIN, 100},
    {ScoreType::CLEAR_MINI_TSPIN_SINGLE, 200},
    {ScoreType::TSPIN, 400},
    {ScoreType::CLEAR_TSPIN_SINGLE, 800},
    {ScoreType::CLEAR_TSPIN_DOUBLE, 1200},
    {ScoreType::CLEAR_TSPIN_TRIPLE, 1600},
    {ScoreType::SOFTDROP, 1},
    {ScoreType::HARDDROP, 2},
    {ScoreType::COMBO, 50},
};

const std::map<ScoreType, const std::string> ScoreTable::score_name = {
    {ScoreType::CLEAR_SINGLE, tr("SINGLE")},
    {ScoreType::CLEAR_DOUBLE, tr("DOUBLE")},
    {ScoreType::CLEAR_TRIPLE, tr("TRIPLE")},
    {ScoreType::CLEAR_PERFECT, tr("PERFECT!")},
    {ScoreType::MINI_TSPIN, tr("T-SPIN MINI")},
    {ScoreType::CLEAR_MINI_TSPIN_SINGLE, tr("T-MINI SINGLE")},
    {ScoreType::TSPIN, tr("T-SPIN")},
    {ScoreType::CLEAR_TSPIN_SINGLE, tr("T-SPIN SINGLE")},
    {ScoreType::CLEAR_TSPIN_DOUBLE, tr("T-SPIN DOUBLE")},
    {ScoreType::CLEAR_TSPIN_TRIPLE, tr("T-SPIN TRIPLE")},
    {ScoreType::COMBO, tr(" COMBO")},
};

float ScoreTable::back2back_multiplier = 1.5;
const std::string ScoreTable::back2back_name = tr("BACK-TO-BACK");


ScoreType ScoreTable::lineclearType(const WellEvent::lineclear_t& lineclear)
{
    ScoreType score_type = ScoreType::SOFTDROP; // a dummy value
    switch(lineclear.type) {
        case LineClearType::NORMAL:
            switch(lineclear.count) {
                case 1: score_type = ScoreType::CLEAR_SINGLE; break;
                case 2: score_type = ScoreType::CLEAR_DOUBLE; break;
                case 3: score_type = ScoreType::CLEAR_TRIPLE; break;
                case 4: score_type = ScoreType::CLEAR_PERFECT; break;
            }
            break;
        case LineClearType::MINI_TSPIN:
            if (lineclear.count == 1)
                score_type = ScoreType::CLEAR_MINI_TSPIN_SINGLE;
            else
                score_type = ScoreType::CLEAR_DOUBLE; // corner case: next to the wall
            break;
        case LineClearType::TSPIN:
            switch(lineclear.count) {
                case 1: score_type = ScoreType::CLEAR_TSPIN_SINGLE; break;
                case 2: score_type = ScoreType::CLEAR_TSPIN_DOUBLE; break;
                case 3: score_type = ScoreType::CLEAR_TSPIN_TRIPLE; break;
            }
            break;
    }
    assert(score_type != ScoreType::SOFTDROP);
    return score_type;
}

bool ScoreTable::canContinueBackToBack(ScoreType previous, ScoreType current)
{
    static const std::set<ScoreType> b2b_allowed = {
        ScoreType::CLEAR_PERFECT,
        ScoreType::CLEAR_MINI_TSPIN_SINGLE,
        ScoreType::CLEAR_TSPIN_SINGLE,
        ScoreType::CLEAR_TSPIN_DOUBLE,
        ScoreType::CLEAR_TSPIN_TRIPLE,
    };

    return b2b_allowed.count(current) && b2b_allowed.count(previous);
}
