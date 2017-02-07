#pragma once

#include <tinydir.h>

#include <string>
#include <vector>
#include <stdexcept>


class TinyDir {
public:
    TinyDir(const std::string& path);
    ~TinyDir();

    std::vector<std::string> fileList();

private:
    tinydir_dir dir;
};


TinyDir::TinyDir(const std::string& path) {
    if (tinydir_open(&dir, path.c_str()) == -1)
        throw std::invalid_argument("Could not open '" + path + "'");
}

TinyDir::~TinyDir() {
    tinydir_close(&dir);
}

std::vector<std::string> TinyDir::fileList()
{
    std::vector<std::string> files;
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        if (!file.is_dir)
            files.emplace_back(file.path);

        tinydir_next(&dir);
    }
    return files;
}
