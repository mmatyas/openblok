#pragma once

#include <string>

enum class ResourceID: uint8_t;

namespace std {
	/// Defines a hasher object for ResourceID,
	/// so we can use it as a hashmap key
    template <>
    struct hash<ResourceID> {
        std::size_t operator() (const ResourceID& elem) const {
            return hash<uint8_t>()(static_cast<uint8_t>(elem));
        }
    };

    /// Defines string coversion for ResourceID
    template<class ResourceID>
    std::string to_string(ResourceID& elem) {
        return std::to_string(static_cast<uint8_t>(elem));
    }
}
