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

TEST_FIXTURE(WellFixture, FromAscii) {
    std::string expected_ascii;
    for (unsigned i = 0; i < 22; i++) {
        if (i % 3 == 0)
            expected_ascii += emptyline_ascii;
        else
            expected_ascii += "SSSSZZZZTT\n";
    }

    well.fromAscii(expected_ascii);
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

    // the piece will reach the bottom
    for (unsigned i = 0; i < 20; i++)
        well.applyGravity();

    std::string expected_ascii;
    for (unsigned i = 0; i < 20; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....ss....\n";
    expected_ascii += "...ss.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());

    // it will lock there
    well.applyGravity();
    expected_ascii = "";
    for (unsigned i = 0; i < 20; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....SS....\n";
    expected_ascii += "...SS.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
    CHECK(well.activePiece() == nullptr);

    // a new piece will appear at the top
    well.addPiece(Piece::Type::Z);
    CHECK(well.activePiece() != nullptr);

    expected_ascii =  "...zz.....\n";
    expected_ascii += "....zz....\n";
    for (unsigned i = 0; i < 16; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...gg.....\n";
    expected_ascii += "....gg....\n";
    expected_ascii += "....SS....\n";
    expected_ascii += "...SS.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());

    // and land on top of the previous
    for (unsigned i = 0; i < 19; i++)
        well.applyGravity();

    expected_ascii = "";
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...ZZ.....\n";
    expected_ascii += "....ZZ....\n";
    expected_ascii += "....SS....\n";
    expected_ascii += "...SS.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
    CHECK(well.activePiece() == nullptr);
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
