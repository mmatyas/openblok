#include "UnitTest++/UnitTest++.h"

#include "game/Piece.h"

#include <string>
#include <iostream>


SUITE(Piece) {

struct PieceFixture {
    Piece p;

    PieceFixture()
        : p(Piece::Type::I, {std::bitset<16>("0000000000000000"),
                             std::bitset<16>("0000000000010000"),
                             std::bitset<16>("0000000100000000"),
                             std::bitset<16>("0001000000000000")})
    {}
};

TEST_FIXTURE(PieceFixture, CtorFirstFrame)
{
    const auto& grid = p.currentGrid();

    for (const auto& row : grid) {
        for (const auto& col : row) {
            REQUIRE CHECK(col.get() == nullptr);
        }
    }
}

TEST_FIXTURE(PieceFixture, RotateRight)
{
    p.rotateRight();
    const auto& grid = p.currentGrid();

    for (unsigned row = 0; row < 4; row++) {
        for (unsigned col = 0; col < 4; col++) {
            if (row == 2 && col == 3)
                CHECK(grid[row][col].get() != nullptr);
            else
                CHECK(grid[row][col].get() == nullptr);
        }
    }
}

TEST_FIXTURE(PieceFixture, RotateLeft)
{
    p.rotateLeft();
    const auto& grid = p.currentGrid();

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
        p.rotateLeft();
        p.rotateRight();
        const auto& grid = p.currentGrid();

        for (const auto& row : grid) {
            for (const auto& col : row) {
                REQUIRE CHECK(col.get() == nullptr);
            }
        }
    }

    {
        p.rotateRight();
        p.rotateLeft();
        const auto& grid = p.currentGrid();

        for (const auto& row : grid) {
            for (const auto& col : row) {
                REQUIRE CHECK(col.get() == nullptr);
            }
        }
    }
}

TEST_FIXTURE(PieceFixture, RotateAround)
{
    {
        p.rotateLeft();
        p.rotateLeft();
        p.rotateLeft();
        p.rotateLeft();
        const auto& grid = p.currentGrid();

        for (const auto& row : grid) {
            for (const auto& col : row) {
                REQUIRE CHECK(col.get() == nullptr);
            }
        }
    }

    {
        p.rotateRight();
        p.rotateRight();
        p.rotateRight();
        p.rotateRight();
        const auto& grid = p.currentGrid();

        for (const auto& row : grid) {
            for (const auto& col : row) {
                REQUIRE CHECK(col.get() == nullptr);
            }
        }
    }
}

} // Suite
