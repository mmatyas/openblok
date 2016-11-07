#pragma once

#include <string>


class Paths {
public:
    Paths() = delete;
    static const std::string& data() { return datadir_path; }

private:
    static const std::string datadir_path;
};
