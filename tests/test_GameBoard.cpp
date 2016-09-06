#include "UnitTest++/UnitTest++.h"

#include "game/GameBoard.h"


SUITE(GameBoard) {

TEST(EmptyOnCreate) {
    GameBoard board;

    std::string emptyness;
    for (unsigned i = 0; i < 22; i++) {
        for (unsigned i = 0; i < 10; i++)
            emptyness += '.';
        emptyness += '\n';
    }

    CHECK_EQUAL(emptyness, board.asAscii());
}

} // Suite
