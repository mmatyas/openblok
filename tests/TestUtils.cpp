#include "TestUtils.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace TestUtils {

std::string run_command(const std::string& cmd) {
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
        throw std::runtime_error("Could not call external command");

    char buffer[128];
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }

    return result;
}

bool imageCompare(const std::string& path1, const std::string& path2, double max_difference)
{
    // TODO: multiplatform support
    try {
        auto result = run_command("compare -metric NCC " + path1 + " " + path2 + " diff.png 2>&1");
        double value = std::stod(result); // throws invalid_argument on error
        return std::fabs(1.0 - value) <= max_difference;
    }
    catch (const std::invalid_argument& err) {
        std::cerr << "Could not parse comparison output\n";
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what();
    }

    return false;
}

} // namespace TestUtils
