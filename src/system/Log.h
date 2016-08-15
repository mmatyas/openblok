#pragma once

#include <ostream>
#include <string>

class Log {
public:
    static std::ostream& info(const std::string& tag);
    static std::ostream& warning(const std::string& tag);
    static std::ostream& error(const std::string& tag);
    static std::ostream& debug(const std::string& tag);
};
