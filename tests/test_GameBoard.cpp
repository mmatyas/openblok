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
    board.addPiece(Piece::Type::I);
    CHECK(board.activePiece() != nullptr);

    std::string expected_ascii = emptyline_ascii;
    expected_ascii += "...iiii...\n";
    for (unsigned i = 0; i < 20; i++)
        expected_ascii += emptyline_ascii;

    CHECK_EQUAL(expected_ascii, board.asAscii());
}

} // Suite
