#include "UnitTest++/UnitTest++.h"

#include "game/GameBoard.h"


SUITE(GameBoard) {

struct BoardFixture {
    GameBoard board;
    std::string emptyline_ascii;

    BoardFixture() {
        for (unsigned i = 0; i < 10; i++)
            emptyline_ascii += '.';
    }
};

TEST_FIXTURE(BoardFixture, EmptyOnCreate) {
    std::string expected_ascii;
    for (unsigned i = 0; i < 22; i++)
        expected_ascii += emptyline_ascii + '\n';

    CHECK_EQUAL(expected_ascii, board.asAscii());
}

} // Suite
