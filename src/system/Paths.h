#pragma once

#include <string>


class Paths {
public:
    Paths() = delete;
    static const std::string& data() { return datadir_path; }
    static const std::string& config() { return configdir_path; }

    static void changeDataDir(const std::string& dir);

private:
    static std::string datadir_path;
    static const std::string configdir_path;
};
