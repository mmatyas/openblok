#pragma once

// For regular mod() on negative numbers, the sign of the remainder is also negative (C++11).
// For ring vectors, we want to stay positive (unsigned).
inline int circularModulo(int num, int mod) {
    return (num % mod + mod) % mod;
}
