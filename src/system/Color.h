#pragma once

#include <stdint.h>


struct RGBColor {
    uint8_t r, g, b;
};

struct RGBAColor {
    uint8_t r, g, b, a;
};

// C++11 custom literals
RGBColor operator "" _rgb(unsigned long long color);
RGBAColor operator "" _rgba(unsigned long long color);
