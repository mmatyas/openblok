#include "UnitTest++/UnitTest++.h"

#include "game/components/MinoStorage.h"
#include "game/components/PieceFactory.h"
#include "game/components/Well.h"
#include "game/components/WellConfig.h"
#include "game/components/rotations/TGM.h"


SUITE(WellTGM) {

// TODO: get these values from config
constexpr unsigned softdrop_delay_frames = 64 / 20.0;

struct WellFixture {
    std::unique_ptr<Well> well;
    std::string emptyline_ascii;

    WellFixture() {
        WellConfig cfg;
        cfg.instant_harddrop = false;
        well = std::make_unique<Well>(std::move(cfg));

        MinoStorage::loadDummyMinos();

        for (unsigned i = 0; i < 10; i++)
            emptyline_ascii += '.';
        emptyline_ascii += '\n';

        PieceFactory::changeInitialPositions(Rotations::TGM().initialPositions());
        well->setRotationFn(std::make_unique<Rotations::TGM>());
    }
};

TEST_FIXTURE(WellFixture, ConspiracyL)
{
    std::string base_ascii;
    for (unsigned i = 0; i < 18; i++)
        base_ascii += emptyline_ascii;
    base_ascii += "OO...OOOOO\n";
    base_ascii += "OO...OOOOO\n";
    base_ascii += "OO.....OOO\n";
    base_ascii += "OO.O.OOOOO\n";

    well->fromAscii(base_ascii);
    well->addPiece(PieceType::L);

    // rotate CW
    well->update({InputEvent(InputType::GAME_ROTATE_RIGHT, true)});
    well->update({InputEvent(InputType::GAME_ROTATE_RIGHT, false)});

    // drop to bottom
    well->update({InputEvent(InputType::GAME_HARDDROP, true)});
    well->update({InputEvent(InputType::GAME_SOFTDROP, false)});
    CHECK(well->activePiece() != nullptr);

    // check position
    std::string expected_ascii;
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "OO...OOOOO\n";
    expected_ascii += "OO.llOOOOO\n";
    expected_ascii += "OO..l..OOO\n";
    expected_ascii += "OO.OlOOOOO\n";
    CHECK_EQUAL(expected_ascii, well->asAscii());


    // rotate CW
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, true)});
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, false)});

    // check position
    expected_ascii.clear();
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "OO...OOOOO\n";
    expected_ascii += "OO...OOOOO\n";
    expected_ascii += "OO..lllOOO\n";
    expected_ascii += "OO.OlOOOOO\n";
    CHECK_EQUAL(expected_ascii, well->asAscii());
}

TEST_FIXTURE(WellFixture, ConspiracyJ)
{
    std::string base_ascii;
    for (unsigned i = 0; i < 18; i++)
        base_ascii += emptyline_ascii;
    base_ascii += "OOOO...OOO\n";
    base_ascii += "OOOO...OOO\n";
    base_ascii += "OO.....OOO\n";
    base_ascii += "OOOO.O.OOO\n";

    well->fromAscii(base_ascii);
    well->addPiece(PieceType::J);

    // rotate CW
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, true)});
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, false)});

    // drop to bottom
    well->update({InputEvent(InputType::GAME_HARDDROP, true)});
    well->update({InputEvent(InputType::GAME_SOFTDROP, false)});
    REQUIRE CHECK(well->activePiece() != nullptr);

    // check position
    std::string expected_ascii;
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "OOOO...OOO\n";
    expected_ascii += "OOOOjj.OOO\n";
    expected_ascii += "OO..j..OOO\n";
    expected_ascii += "OOOOjO.OOO\n";
    REQUIRE CHECK_EQUAL(expected_ascii, well->asAscii());


    // rotate CW
    well->update({InputEvent(InputType::GAME_ROTATE_RIGHT, true)});
    well->update({InputEvent(InputType::GAME_ROTATE_RIGHT, false)});

    // check position
    expected_ascii.clear();
    for (unsigned i = 0; i < 18; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "OOOO...OOO\n";
    expected_ascii += "OOOO...OOO\n";
    expected_ascii += "OO..jjjOOO\n";
    expected_ascii += "OOOO.OjOOO\n";
    REQUIRE CHECK_EQUAL(expected_ascii, well->asAscii());
}

TEST_FIXTURE(WellFixture, ZKick)
{
    std::string base_ascii;
    for (unsigned i = 0; i < 19; i++)
        base_ascii += emptyline_ascii;
    base_ascii += "OOOO..OOOO\n";
    base_ascii += "OO....OOOO\n";
    base_ascii += "OOO..OOOOO\n";

    well->fromAscii(base_ascii);
    well->addPiece(PieceType::Z);

    // rotate CW
    well->update({InputEvent(InputType::GAME_ROTATE_RIGHT, true)});
    well->update({InputEvent(InputType::GAME_ROTATE_RIGHT, false)});

    // drop to bottom
    well->update({InputEvent(InputType::GAME_HARDDROP, true)});
    well->update({InputEvent(InputType::GAME_SOFTDROP, false)});
    REQUIRE CHECK(well->activePiece() != nullptr);

    // check position
    std::string expected_ascii;
    for (unsigned i = 0; i < 19; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "OOOO.zOOOO\n";
    expected_ascii += "OO..zzOOOO\n";
    expected_ascii += "OOO.zOOOOO\n";
    REQUIRE CHECK_EQUAL(expected_ascii, well->asAscii());


    // rotate CW
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, true)});
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, false)});

    // check position
    expected_ascii.clear();
    for (unsigned i = 0; i < 19; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "OOOO..OOOO\n";
    expected_ascii += "OOzz..OOOO\n";
    expected_ascii += "OOOzzOOOOO\n";
    REQUIRE CHECK_EQUAL(expected_ascii, well->asAscii());
}

TEST_FIXTURE(WellFixture, ZKick2)
{
    std::string base_ascii;
    for (unsigned i = 0; i < 19; i++)
        base_ascii += emptyline_ascii;
    base_ascii += "OOOO..O.OO\n";
    base_ascii += "OOOO....OO\n";
    base_ascii += "OOOO...OOO\n";

    well->fromAscii(base_ascii);
    well->addPiece(PieceType::Z);

    // rotate CW
    well->update({InputEvent(InputType::GAME_ROTATE_RIGHT, true)});
    well->update({InputEvent(InputType::GAME_ROTATE_RIGHT, false)});

    // drop to bottom
    well->update({InputEvent(InputType::GAME_HARDDROP, true)});
    well->update({InputEvent(InputType::GAME_SOFTDROP, false)});
    REQUIRE CHECK(well->activePiece() != nullptr);

    // check position
    std::string expected_ascii;
    for (unsigned i = 0; i < 19; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "OOOO.zO.OO\n";
    expected_ascii += "OOOOzz..OO\n";
    expected_ascii += "OOOOz..OOO\n";
    REQUIRE CHECK_EQUAL(expected_ascii, well->asAscii());


    // rotate CW 2x
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, true)});
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, false)});
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, true)});
    well->update({InputEvent(InputType::GAME_ROTATE_LEFT, false)});

    // check position
    expected_ascii.clear();
    for (unsigned i = 0; i < 19; i++)
        expected_ascii += emptyline_ascii;
    expected_ascii += "OOOO..OzOO\n";
    expected_ascii += "OOOO..zzOO\n";
    expected_ascii += "OOOO..zOOO\n";
    REQUIRE CHECK_EQUAL(expected_ascii, well->asAscii());
}

} // Suite
