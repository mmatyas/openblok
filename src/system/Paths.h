#pragma once

#include <string>


class Paths {
public:
    Paths() = delete;
    static const std::string& data() { return datadir_path; }
    static const std::string& config() { return configdir_path; }

private:
    static const std::string datadir_path;
    static const std::string configdir_path;
};
