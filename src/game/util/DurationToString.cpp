#include "DurationToString.h"

namespace Timing
{

const std::string toString(Duration duration)
{
    unsigned minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
    unsigned seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 60;
    std::string str;
    if (minutes < 10)
        str += "0";
    str += std::to_string(minutes);
    str += ":";
    if (seconds < 10)
        str += "0";
    str += std::to_string(seconds);
    return str;
}

} // namespace Timing
