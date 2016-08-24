#pragma once

#include <string>

namespace TestUtils {

bool imageCompare(const std::string& path1, const std::string& path2, double max_difference = 0.001);

} // namespace TestUtils
