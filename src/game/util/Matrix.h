#pragma once

#include <array>
#include <stddef.h>


template <class T, size_t ROW, size_t COL>
using Matrix = std::array<std::array<T, COL>, ROW>;
