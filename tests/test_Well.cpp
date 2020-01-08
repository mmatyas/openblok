#include "UnitTest++/UnitTest++.h"

#include "game/WellConfig.h"
#include "game/components/MinoStorage.h"
#include "game/components/PieceType.h"
#include "game/components/PieceFactory.h"
#include "game/components/Well.h"
#include "game/components/rotations/SRS.h"


SUITE(Well) {

// TODO: get these values from config
constexpr unsigned gravity_delay_frames = 64;
constexpr unsigned horizontal_delay_frames = 14;

struct WellFixture {
    Well well;
    std::string emptyline_ascii;

    WellFixture() {
        MinoStorage::loadDummyMinos();

        for (unsigned i = 0; i < 10; i++)
            emptyline_ascii += '.';
        emptyline_ascii += '\n';

        PieceFactory::changeInitialPositions(Rotations::SRS().initialPositions());
    }
};

TEST_FIXTURE(WellFixture, EmptyOnCreate) {
    std::string expected_ascii;
    for (unsigned i = 0; i < 22; i++)
        expected_ascii += emptyline_ascii;

    CHECK_EQUAL(expected_ascii, well.asAscii());
    CHECK(well.activePiece() == nullptr);
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
    well.addPiece(PieceType::S);
    REQUIRE CHECK(well.activePiece() != nullptr);

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
    well.addPiece(PieceType::S);
    REQUIRE CHECK(well.activePiece() != nullptr);

    // the piece will reach the bottom
    for (unsigned i = 0; i < 18 * gravity_delay_frames; i++)
        well.update({});

    std::string expected_ascii;
    expected_ascii += emptyline_ascii;
    expected_ascii += emptyline_ascii;
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....ss....\n";
    expected_ascii += "...ss.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());

    // it will lock here
    unsigned lock_limit = 0;
    while (lock_limit < 100 && well.activePiece()) {
        well.update({});
        lock_limit++;
    }
    REQUIRE CHECK(well.activePiece() == nullptr);

    expected_ascii = "";
    for (unsigned i = 0; i < 20; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....SS....\n";
    expected_ascii += "...SS.....\n";
    CHECK_EQUAL(expected_ascii, well.asAscii());

    // a new piece will appear at the top
    well.addPiece(PieceType::Z);
    well.update({});
    REQUIRE CHECK(well.activePiece() != nullptr);

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
    for (unsigned i = 0; i < 17 * gravity_delay_frames && well.activePiece(); i++)
        well.update({});
    CHECK(well.activePiece() == nullptr);

    expected_ascii = "";
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "...ZZ.....\n";
    expected_ascii += "....ZZ....\n";
    expected_ascii += "....SS....\n";
    expected_ascii += "...SS.....\n";

    CHECK_EQUAL(expected_ascii, well.asAscii());
}


TEST_FIXTURE(WellFixture, MoveLeft) {
    well.addPiece(PieceType::I);
    well.update({InputEvent(InputType::GAME_MOVE_LEFT, true)});

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
    well.update({InputEvent(InputType::GAME_MOVE_RIGHT, true)});

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
    well.update({InputEvent(InputType::GAME_SOFTDROP, true)});

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

    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, true)});

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

TEST_FIXTURE(WellFixture, Clear) {
    well.addPiece(PieceType::I);
    for (unsigned i = 0; i < horizontal_delay_frames * 3; i++)
        well.update({InputEvent(InputType::GAME_MOVE_LEFT, true)});
    well.update({
        InputEvent(InputType::GAME_MOVE_LEFT, false),
        InputEvent(InputType::GAME_HARDDROP, true),
        InputEvent(InputType::GAME_HARDDROP, false),
    });
    CHECK(well.activePiece() == nullptr);

    well.addPiece(PieceType::I);
    for (unsigned i = 0; i < horizontal_delay_frames * 3; i++)
        well.update({InputEvent(InputType::GAME_MOVE_RIGHT, true)});
    well.update({
        InputEvent(InputType::GAME_MOVE_RIGHT, false),
        InputEvent(InputType::GAME_HARDDROP, true),
        InputEvent(InputType::GAME_HARDDROP, false),
    });
    CHECK(well.activePiece() == nullptr);

    well.addPiece(PieceType::O);
    well.update({
        InputEvent(InputType::GAME_HARDDROP, true),
        InputEvent(InputType::GAME_HARDDROP, false),
    });
    CHECK(well.activePiece() == nullptr);

    constexpr unsigned CLEAR_DELAY_FRAMES = 41;
    for (unsigned i = 0; i < CLEAR_DELAY_FRAMES; i++)
        well.update({});

    std::string expected_ascii;
    for (unsigned i = 0; i < 21; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "....OO....\n";
    CHECK_EQUAL(expected_ascii, well.asAscii());
}

TEST(Zangi) {
    std::string emptyline_ascii;
    MinoStorage::loadDummyMinos();
    for (unsigned i = 0; i < 10; i++)
        emptyline_ascii += '.';
    emptyline_ascii += '\n';

    WellConfig cfg;
    cfg.instant_harddrop = false;
    Well well(std::move(cfg));


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

    well.addPiece(PieceType::J);
    // rotate CCW
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, true)});
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, false)});
    // move right
    for (unsigned i = 0; i < horizontal_delay_frames * 4; i++)
        well.update({InputEvent(InputType::GAME_MOVE_RIGHT, true)});
    well.update({InputEvent(InputType::GAME_MOVE_RIGHT, false)});
    // sonic drop
    well.update({InputEvent(InputType::GAME_HARDDROP, true)});
    well.update({InputEvent(InputType::GAME_HARDDROP, false)});
    CHECK(well.activePiece() != nullptr); // piece must survive landing on sonic drop
    // move left
    well.update({InputEvent(InputType::GAME_MOVE_LEFT, true)});
    CHECK(well.activePiece() != nullptr);
    well.update({InputEvent(InputType::GAME_MOVE_LEFT, false)});
    CHECK(well.activePiece() != nullptr);
    // lock
    well.update({InputEvent(InputType::GAME_SOFTDROP, true)});
    CHECK(well.activePiece() == nullptr);
    well.update({InputEvent(InputType::GAME_SOFTDROP, false)});
    CHECK(well.activePiece() == nullptr);

    CHECK_EQUAL(expected_ascii, well.asAscii());
}

} // Suite
