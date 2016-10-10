#include "UnitTest++/UnitTest++.h"

#include "game/components/PieceType.h"
#include "game/components/PieceFactory.h"
#include "game/components/Well.h"


class AppContext {};

SUITE(Well) {

// TODO: get these values from config
constexpr unsigned gravity_delay_frames = 64;
constexpr unsigned lock_delay_frames = 30;
constexpr unsigned horizontal_delay_frames = 14;

struct WellFixture {
    AppContext app;
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
    well.addPiece(PieceType::S);
    CHECK(well.activePiece() != nullptr);

    std::string expected_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += "....ss....\n";
    expected_ascii += "...ss.....\n";
    for (unsigned i = 0; i < 16; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....gg....\n";
    expected_ascii += "...gg.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
}

TEST_FIXTURE(WellFixture, Gravity) {
    CHECK(well.activePiece() == nullptr);
    well.addPiece(PieceType::S);
    CHECK(well.activePiece() != nullptr);

    // the piece will reach the bottom
    for (unsigned i = 0; i < 18 * gravity_delay_frames; i++)
        well.update({}, app);

    std::string expected_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += emptyline_ascii;
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....ss....\n";
    expected_ascii += "...ss.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());

    // it will lock there
    for (unsigned i = 0; i < lock_delay_frames; i++)
        well.update({}, app);

    expected_ascii = "";
    for (unsigned i = 0; i < 20; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....SS....\n";
    expected_ascii += "...SS.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
    CHECK(well.activePiece() == nullptr);

    // a new piece will appear at the top
    well.addPiece(PieceType::Z);
    well.update({}, app);
    CHECK(well.activePiece() != nullptr);

    expected_ascii = emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii +=  "...zz.....\n";
    expected_ascii += "....zz....\n";
    for (unsigned i = 0; i < 14; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...gg.....\n";
    expected_ascii += "....gg....\n";
    expected_ascii += "....SS....\n";
    expected_ascii += "...SS.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());

    // and land on top of the previous
    for (unsigned i = 0; i < 17 * gravity_delay_frames; i++)
        well.update({}, app);

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


TEST_FIXTURE(WellFixture, MoveLeft) {
    well.addPiece(PieceType::I);
    well.update({InputEvent(InputType::LEFT, true)}, app);

    std::string expected_ascii = emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += "..iiii....\n";
    for (unsigned i = 0; i < 17; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "..gggg....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
}
TEST_FIXTURE(WellFixture, MoveRight) {
    well.addPiece(PieceType::I);
    well.update({InputEvent(InputType::RIGHT, true)}, app);

    std::string expected_ascii = emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += "....iiii..\n";
    for (unsigned i = 0; i < 17; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....gggg..\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
}
TEST_FIXTURE(WellFixture, MoveDown) {
    well.addPiece(PieceType::I);
    well.update({InputEvent(InputType::DOWN, true)}, app);

    std::string expected_ascii = emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += "...iiii...\n";
    for (unsigned i = 0; i < 16; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...gggg...\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
}

TEST_FIXTURE(WellFixture, Rotate) {
    CHECK(well.activePiece() == nullptr);
    well.addPiece(PieceType::S);
    CHECK(well.activePiece() != nullptr);

    well.update({InputEvent(InputType::A, true)}, app);

    std::string expected_ascii = emptyline_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += "...s......\n";
    expected_ascii += "...ss.....\n";
    expected_ascii += "....s.....\n";
    for (unsigned i = 0; i < 14; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...g......\n";
    expected_ascii += "...gg.....\n";
    expected_ascii += "....g.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
}

// TODO: Make Well configurable
/*
TEST_FIXTURE(WellFixture, Zangi) {
    std::string base_ascii;
    for (unsigned i = 0; i < 18; i++)
        base_ascii += emptyline_ascii;
    base_ascii += "TTTTTTTT..\n";
    base_ascii += "TTTTTTTT..\n";
    base_ascii += "TTTTTTT...\n";
    base_ascii += "TTTTTTTTT.\n";

    std::string expected_ascii;
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "TTTTTTTTJ.\n";
    expected_ascii += "TTTTTTTTJ.\n";
    expected_ascii += "TTTTTTTJJ.\n";
    expected_ascii += "TTTTTTTTT.\n";

    well.fromAscii(base_ascii);

    well.addPiece(Piece::Type::J);
    // rotate CCW
    well.update({InputEvent(InputType::A, true)}, app);
    well.update({InputEvent(InputType::A, false)}, app);
    // move right
    for (unsigned i = 0; i < horizontal_delay_frames * 4; i++)
        well.update({InputEvent(InputType::RIGHT, true)}, app);
    well.update({InputEvent(InputType::RIGHT, false)}, app);
    // sonic drop
    well.update({InputEvent(InputType::UP, true)}, app);
    well.update({InputEvent(InputType::UP, false)}, app);
    CHECK(well.activePiece() != nullptr); // piece must survive landing on sonic drop
    // move left
    well.update({InputEvent(InputType::LEFT, true)}, app);
    CHECK(well.activePiece() != nullptr);
    well.update({InputEvent(InputType::LEFT, false)}, app);
    CHECK(well.activePiece() != nullptr);
    // lock
    well.update({InputEvent(InputType::DOWN, true)}, app);
    CHECK(well.activePiece() == nullptr);
    well.update({InputEvent(InputType::DOWN, false)}, app);
    CHECK(well.activePiece() == nullptr);

    CHECK_EQUAL(expected_ascii, well.asAscii());
}
*/

} // Suite
