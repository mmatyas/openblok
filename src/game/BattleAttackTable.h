#pragma once

#include "WellEvent.h"

class BattleAttackTable {
public:
    static unsigned sendableLineCount(const WellEvent::lineclear_t&, bool is_b2b);

};
