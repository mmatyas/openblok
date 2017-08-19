#include "UnitTest++/UnitTest++.h"

#include "game/components/MinoStorage.h"
#include "game/components/Piece.h"
#include "game/components/PieceType.h"
#include "system/util/MakeUnique.h"

#include <bitset>
#include <string>
#include <memory>


SUITE(Piece) {

struct PieceFixture {
    std::unique_ptr<Piece> p;
    const std::array<std::bitset<16>, 4> grid;

    PieceFixture()
        : grid({{std::bitset<16>("1111111111111111"),
                 std::bitset<16>("0000000000010000"),
                 std::bitset<16>("0000000100000000"),
                 std::bitset<16>("0001000000000000")}})
    {
        MinoStorage::loadDummyMinos();
        p = std::make_unique<Piece>(PieceType::I, grid);
    }
};

TEST_FIXTURE(PieceFixture, CtorFirstFrame)
{
    const auto& grid = p->currentGrid();

    for (const auto& row : grid) {
        for (const auto& col : row) {
            REQUIRE CHECK(col.get() != nullptr);
        }
    }
}

TEST_FIXTURE(PieceFixture, RotateCW)
{
    p->rotateCW();
    const auto& grid = p->currentGrid();

    for (unsigned row = 0; row < 4; row++) {
        for (unsigned col = 0; col < 4; col++) {
            if (row == 2 && col == 3)
                CHECK(grid[row][col].get() != nullptr);
            else
                CHECK(grid[row][col].get() == nullptr);
        }
    }
}

TEST_FIXTURE(PieceFixture, RotateCCW)
{
    p->rotateCCW();
    const auto& grid = p->currentGrid();

    for (unsigned row = 0; row < 4; row++) {
        for (unsigned col = 0; col < 4; col++) {
            if (row == 0 && col == 3)
                CHECK(grid[row][col].get() != nullptr);
            else
                CHECK(grid[row][col].get() == nullptr);
        }
    }
}

TEST_FIXTURE(PieceFixture, RotateBack)
{
    {
        p->rotateCCW();
        p->rotateCW();
        const auto& grid = p->currentGrid();

        for (const auto& row : grid) {
            for (const auto& col : row) {
                REQUIRE CHECK(col.get() != nullptr);
            }
        }
    }

    {
        p->rotateCW();
        p->rotateCCW();
        const auto& grid = p->currentGrid();

        for (const auto& row : grid) {
            for (const auto& col : row) {
                REQUIRE CHECK(col.get() != nullptr);
            }
        }
    }
}

TEST_FIXTURE(PieceFixture, RotateAround)
{
    {
        p->rotateCCW();
        p->rotateCCW();
        p->rotateCCW();
        p->rotateCCW();
        const auto& grid = p->currentGrid();

        for (const auto& row : grid) {
            for (const auto& col : row) {
                REQUIRE CHECK(col.get() != nullptr);
            }
        }
    }

    {
        p->rotateCW();
        p->rotateCW();
        p->rotateCW();
        p->rotateCW();
        const auto& grid = p->currentGrid();

        for (const auto& row : grid) {
            for (const auto& col : row) {
                REQUIRE CHECK(col.get() != nullptr);
            }
        }
    }
}

} // Suite
