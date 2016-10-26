#pragma once

#include <string>


class Well;


namespace WellComponents {

class Ascii {
public:
    /// Get the well's string representation.
    /// Can be useful for testing and debugging.
    void fromAscii(Well&, const std::string& text);
    /// Set the contents of the well from an Ascii string.
    std::string asAscii(const Well&) const;
};

} // namespace WellComponents
