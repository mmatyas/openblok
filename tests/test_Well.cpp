#include "UnitTest++/UnitTest++.h"

#include "game/Well.h"
#include "game/PieceFactory.h"


SUITE(Well) {

struct WellFixture {
    Well well;
    std::string emptyline_ascii;

    WellFixture() {
        for (unsigned i = 0; i < 10; i++)
            emptyline_ascii += '.';
        emptyline_ascii += '\n';
    }
};

TEST_FIXTURE(WellFixture, EmptyOnCreate) {
    std::string expected_ascii;
    for (unsigned i = 0; i < 22; i++)
        expected_ascii += emptyline_ascii;

    CHECK_EQUAL(expected_ascii, well.asAscii());
}

TEST_FIXTURE(WellFixture, AddPiece) {
    CHECK(well.activePiece() == nullptr);
    well.addPiece(Piece::Type::S);
    CHECK(well.activePiece() != nullptr);

    std::string expected_ascii;
    expected_ascii += "....ss....\n";
    expected_ascii += "...ss.....\n";
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....gg....\n";
    expected_ascii += "...gg.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
}

TEST_FIXTURE(WellFixture, Gravity) {
    CHECK(well.activePiece() == nullptr);
    well.addPiece(Piece::Type::S);
    CHECK(well.activePiece() != nullptr);

    for (unsigned i = 0; i < 20; i++)
        well.applyGravity();

    std::string expected_ascii;
    for (unsigned i = 0; i < 20; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....ss....\n";
    expected_ascii += "...ss.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
}


TEST_FIXTURE(WellFixture, Move) {
    CHECK(well.activePiece() == nullptr);
    well.addPiece(Piece::Type::I);
    CHECK(well.activePiece() != nullptr);

    well.moveLeftNow();

    std::string expected_ascii = emptyline_ascii;
    expected_ascii += "..iiii....\n";
    for (unsigned i = 0; i < 19; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "..gggg....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());


    well.moveRightNow();

    expected_ascii = emptyline_ascii;
    expected_ascii += "...iiii...\n";
    for (unsigned i = 0; i < 19; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...gggg...\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());


    well.moveDownNow();

    expected_ascii = emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += "...iiii...\n";
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...gggg...\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
}

} // Suite
