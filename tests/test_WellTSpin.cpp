#include "UnitTest++/UnitTest++.h"

#include "game/AppContext.h"
#include "game/components/MinoStorage.h"
#include "game/components/Well.h"


SUITE(WellTSpin) {

// TODO: get these values from config
constexpr unsigned softdrop_delay_frames = 64 / 20.0;

struct WellFixture {
    AppContext app;
    Well well;
    std::string emptyline_ascii;

    WellFixture() {
        MinoStorage::loadDummyMinos();

        for (unsigned i = 0; i < 10; i++)
            emptyline_ascii += '.';
        emptyline_ascii += '\n';

        well.setRotationFn(app.scripts().loadRotationFn("data/rotations/srs.lua"));
    }
};

TEST_FIXTURE(WellFixture, Basic)
{
    bool tspin_detected = false;
    well.registerObserver(WellEvent::Type::LINE_CLEAR, [&tspin_detected](const WellEvent& event){
        if (event.lineclear.type == LineClearType::TSPIN && event.lineclear.count == 2)
            tspin_detected = true;
    });

    std::string base_ascii;
    for (unsigned i = 0; i < 19; i++)
        base_ascii += emptyline_ascii;
    base_ascii += ".....OO...\n";
    base_ascii += "OOO...OOOO\n";
    base_ascii += "OOOO.OOOOO\n";

    well.fromAscii(base_ascii);
    well.addPiece(PieceType::T);

    // rotate CCW
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, true)}, app);
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, false)}, app);

    // move to bottom
    for (unsigned row = 0; row < 22 * softdrop_delay_frames; row++) {
        well.update({InputEvent(InputType::GAME_SOFTDROP, true)}, app);
    }
    well.update({InputEvent(InputType::GAME_SOFTDROP, false)}, app);
    CHECK(well.activePiece() != nullptr);

    // rotate CCW
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, true)}, app);
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, false)}, app);

    // check position
    std::string expected_ascii;
    for (unsigned i = 0; i < 19; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += ".....OO...\n";
    expected_ascii += "OOOtttOOOO\n";
    expected_ascii += "OOOOtOOOOO\n";
    CHECK_EQUAL(expected_ascii, well.asAscii());

    // wait for lock
    unsigned limit = 0;
    while (limit < 100) {
        well.update({}, app);
        limit++;
    }

    CHECK(well.activePiece() == nullptr);
    CHECK_EQUAL(true, tspin_detected);
}

TEST_FIXTURE(WellFixture, Tricky)
{
    bool tspin_detected = false;
    well.registerObserver(WellEvent::Type::LINE_CLEAR, [&tspin_detected](const WellEvent& event){
        if (event.lineclear.type == LineClearType::TSPIN && event.lineclear.count == 3)
            tspin_detected = true;
    });

    std::string base_ascii;
    for (unsigned i = 0; i < 17; i++)
        base_ascii += emptyline_ascii;
    base_ascii += "......OOOO\n";
    base_ascii += ".......OOO\n";
    base_ascii += "OOOOOO.OOO\n";
    base_ascii += "OOOOO..OOO\n";
    base_ascii += "OOOOOO.OOO\n";

    well.fromAscii(base_ascii);
    well.addPiece(PieceType::T);

    // move to bottom
    for (unsigned row = 0; row < 20 * softdrop_delay_frames; row++) {
        well.update({InputEvent(InputType::GAME_SOFTDROP, true)}, app);
    }
    well.update({InputEvent(InputType::GAME_SOFTDROP, false)}, app);
    CHECK(well.activePiece() != nullptr);

    // move right
    well.update({InputEvent(InputType::GAME_MOVE_RIGHT, true)}, app);
    well.update({InputEvent(InputType::GAME_MOVE_RIGHT, false)}, app);

    // rotate CCW
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, true)}, app);
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, false)}, app);

    // check position
    std::string expected_ascii;
    for (unsigned i = 0; i < 17; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "......OOOO\n";
    expected_ascii += ".......OOO\n";
    expected_ascii += "OOOOOOtOOO\n";
    expected_ascii += "OOOOOttOOO\n";
    expected_ascii += "OOOOOOtOOO\n";
    CHECK_EQUAL(expected_ascii, well.asAscii());

    // wait for lock
    unsigned limit = 0;
    while (limit < 100) {
        well.update({}, app);
        limit++;
    }

    CHECK(well.activePiece() == nullptr);
    CHECK_EQUAL(true, tspin_detected);
}

TEST_FIXTURE(WellFixture, TrickyCross)
{
    bool tspin_detected = false;
    well.registerObserver(WellEvent::Type::LINE_CLEAR, [&tspin_detected](const WellEvent& event){
        if (event.lineclear.type == LineClearType::TSPIN && event.lineclear.count == 2)
            tspin_detected = true;
    });

    std::string base_ascii;
    for (unsigned i = 0; i < 17; i++)
        base_ascii += emptyline_ascii;
    base_ascii += "......OOOO\n";
    base_ascii += ".......OOO\n";
    base_ascii += "OOOOOO.OOO\n";
    base_ascii += "OOOOO...OO\n"; // here's the difference!
    base_ascii += "OOOOOO.OOO\n";

    well.fromAscii(base_ascii);
    well.addPiece(PieceType::T);

    // move to bottom
    for (unsigned row = 0; row < 20 * softdrop_delay_frames; row++) {
        well.update({InputEvent(InputType::GAME_SOFTDROP, true)}, app);
    }
    well.update({InputEvent(InputType::GAME_SOFTDROP, false)}, app);
    CHECK(well.activePiece() != nullptr);

    // move right
    well.update({InputEvent(InputType::GAME_MOVE_RIGHT, true)}, app);
    well.update({InputEvent(InputType::GAME_MOVE_RIGHT, false)}, app);

    // rotate CCW
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, true)}, app);
    well.update({InputEvent(InputType::GAME_ROTATE_LEFT, false)}, app);

    // wait for lock
    unsigned limit = 0;
    while (limit < 100) {
        well.update({}, app);
        limit++;
    }

    CHECK(well.activePiece() == nullptr);
    CHECK_EQUAL(true, tspin_detected);
}

TEST_FIXTURE(WellFixture, Mini)
{
    bool tspin_detected = false;
    well.registerObserver(WellEvent::Type::LINE_CLEAR, [&tspin_detected](const WellEvent& event){
        if (event.lineclear.type == LineClearType::MINI_TSPIN && event.lineclear.count == 1)
            tspin_detected = true;
    });

    std::string base_ascii;
    for (unsigned i = 0; i < 19; i++)
        base_ascii += emptyline_ascii;
    base_ascii += "OOO.......\n";
    base_ascii += "OOO.......\n";
    base_ascii += "OOO.OOOOOO\n";

    well.fromAscii(base_ascii);
    well.addPiece(PieceType::T);

    // move to bottom
    for (unsigned row = 0; row < 22 * softdrop_delay_frames; row++) {
        well.update({InputEvent(InputType::GAME_SOFTDROP, true)}, app);
    }
    well.update({InputEvent(InputType::GAME_SOFTDROP, false)}, app);
    CHECK(well.activePiece() != nullptr);

    // rotate CW
    well.update({InputEvent(InputType::GAME_ROTATE_RIGHT, true)}, app);
    well.update({InputEvent(InputType::GAME_ROTATE_RIGHT, false)}, app);

    // wait for lock
    unsigned limit = 0;
    while (limit < 100) {
        well.update({}, app);
        limit++;
    }

    CHECK(well.activePiece() == nullptr);
    CHECK_EQUAL(true, tspin_detected);
}

} // Suite
