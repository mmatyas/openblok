#pragma once

#include <string>
#include <unordered_map>


class ConfigFile {
public:
    using BlockName = std::string;
    using KeyValPairs = std::unordered_map<std::string, std::string>;
    using Data = std::unordered_map<BlockName, KeyValPairs>;

    static Data load(const std::string& path);
    static void save(const Data&, const std::string& path);
};
