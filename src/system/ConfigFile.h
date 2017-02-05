#pragma once

#include <string>
#include <unordered_map>


class ConfigFile {
public:
    using BlockName = std::string;
    using KeyValPairs = std::unordered_map<std::string, std::string>;
    using Blocks = std::unordered_map<BlockName, KeyValPairs>;

    static Blocks load(const std::string& path);
    static void save(const Blocks&, const std::string& path);
};
