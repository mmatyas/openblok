#include "Log.h"

#include <iostream>


std::ostream& Log::info(const std::string& tag)
{
    return std::cout << "[i][" << tag << "] ";
}

std::ostream& Log::warning(const std::string& tag)
{
    return std::cout << "[w][" << tag << "] ";
}

std::ostream& Log::error(const std::string& tag)
{
    return std::cerr << "[e][" << tag << "] ";
}

std::ostream& Log::debug(const std::string& tag)
{
    return std::cout << "[d][" << tag << "] ";
}
