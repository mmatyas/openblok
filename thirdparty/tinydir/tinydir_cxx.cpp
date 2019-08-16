#include "tinydir_cxx.h"

#include <stdexcept>


TinyDir::TinyDir(const std::string& path)
{
    if (tinydir_open(&dir, path.c_str()) == -1)
        throw std::invalid_argument("Could not open '" + path + "'");
};

TinyDir::~TinyDir()
{
    tinydir_close(&dir);
}

std::vector<std::string> TinyDir::file_list()
{
    std::vector<std::string> files;
    while (dir.has_next) {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        if (!file.is_dir)
            files.emplace_back(file.path);
        tinydir_next(&dir);
    }
    return files;
}
