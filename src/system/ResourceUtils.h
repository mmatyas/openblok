#pragma once

#include <string>

enum class FontID: uint8_t;
enum class TexID: uint8_t;

/// Defines a hasher object for resource ids,
/// so we can use them as a hashmap key
struct ResourceIDHash {
    template <typename T>
    constexpr std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

namespace std {
    /// Defines string coversion for resource ids
    template<class FontID>
    constexpr std::string to_string(FontID& elem) {
        return std::to_string(static_cast<uint8_t>(elem));
    }
    // NOTE: for some reason, the template above applies to TexID too
}
