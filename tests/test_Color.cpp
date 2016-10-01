#include "UnitTest++/UnitTest++.h"

#include "system/Color.h"


SUITE(Color) {

TEST(RGB)
{
    auto color = 0xABCDEF_rgb;
    CHECK_EQUAL(0xAB, color.r);
    CHECK_EQUAL(0xCD, color.g);
    CHECK_EQUAL(0xEF, color.b);
}

TEST(RGBA)
{
    auto color = 0xABCDEF77_rgba;
    CHECK_EQUAL(0xAB, color.r);
    CHECK_EQUAL(0xCD, color.g);
    CHECK_EQUAL(0xEF, color.b);
    CHECK_EQUAL(0x77, color.a);
}

} // Suite
