#include "BattleAttackTable.h"

#include <assert.h>


unsigned BattleAttackTable::sendableLineCount(const WellEvent::lineclear_t& event, bool is_b2b)
{
    assert(event.count > 0);

    unsigned lines = 0;
    switch (event.type) {
        case LineClearType::NORMAL:
            assert(event.count <= 4);
            switch (event.count) {
                case 1: lines = 0; break;
                case 2: lines = 1; break;
                case 3: lines = 2; break;
                case 4: lines = 4; break;
                default: assert(false); break;
            }
            break;
        case LineClearType::MINI_TSPIN:
            assert(event.count == 1);
            lines = 0;
            break;
        case LineClearType::TSPIN:
            assert(event.count <= 3);
            lines = event.count * 2;
            break;
    }

    if (is_b2b)
        lines++;

    return lines;
}

