#include "UnitTest++/UnitTest++.h"

#include "game/GameBoard.h"
#include "game/PieceFactory.h"


SUITE(GameBoard) {

struct BoardFixture {
    GameBoard board;
    std::string emptyline_ascii;

    BoardFixture() {
        for (unsigned i = 0; i < 10; i++)
            emptyline_ascii += '.';
        emptyline_ascii += '\n';
    }
};

TEST_FIXTURE(BoardFixture, EmptyOnCreate) {
    std::string expected_ascii;
    for (unsigned i = 0; i < 22; i++)
        expected_ascii += emptyline_ascii;

    CHECK_EQUAL(expected_ascii, board.asAscii());
}

TEST_FIXTURE(BoardFixture, AddPiece) {
    CHECK(board.activePiece() == nullptr);
    board.addPiece(Piece::Type::S);
    CHECK(board.activePiece() != nullptr);

    std::string expected_ascii;
    expected_ascii += "....ss....\n";
    expected_ascii += "...ss.....\n";
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....gg....\n";
    expected_ascii += "...gg.....\n";

    CHECK_EQUAL(expected_ascii, board.asAscii());
}

TEST_FIXTURE(BoardFixture, Gravity) {
    CHECK(board.activePiece() == nullptr);
    board.addPiece(Piece::Type::S);
    CHECK(board.activePiece() != nullptr);

    for (unsigned i = 0; i < 20; i++)
        board.applyGravity();

    std::string expected_ascii;
    for (unsigned i = 0; i < 20; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....ss....\n";
    expected_ascii += "...ss.....\n";

    CHECK_EQUAL(expected_ascii, board.asAscii());
}


TEST_FIXTURE(BoardFixture, Move) {
    CHECK(board.activePiece() == nullptr);
    board.addPiece(Piece::Type::I);
    CHECK(board.activePiece() != nullptr);

    board.moveLeftNow();

    std::string expected_ascii = emptyline_ascii;
    expected_ascii += "..iiii....\n";
    for (unsigned i = 0; i < 19; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "..gggg....\n";

    CHECK_EQUAL(expected_ascii, board.asAscii());


    board.moveRightNow();

    expected_ascii = emptyline_ascii;
    expected_ascii += "...iiii...\n";
    for (unsigned i = 0; i < 19; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...gggg...\n";

    CHECK_EQUAL(expected_ascii, board.asAscii());


    board.moveDownNow();

    expected_ascii = emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += "...iiii...\n";
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...gggg...\n";

    CHECK_EQUAL(expected_ascii, board.asAscii());
}

} // Suite
