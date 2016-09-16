#pragma once

#include <stdint.h>


struct RGBColor {
    uint8_t r, g, b;
};

// C++11 custom literal
RGBColor operator "" _rgb(unsigned long long color);
