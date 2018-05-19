#pragma once

#include <tinydir.h>

#include <string>
#include <vector>
#include <stdexcept>

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#define posix_access _access
#else
#include <unistd.h>
#define posix_access access
#endif

#if defined _MSC_VER || defined __MINGW32__
#undef INFINITE
#endif


inline bool path_exists(const std::string& path) {
    return posix_access(path.c_str(), 0) == 0;
}


class TinyDir {
public:
    TinyDir(const std::string& path) {
        if (tinydir_open(&dir, path.c_str()) == -1)
            throw std::invalid_argument("Could not open '" + path + "'");
    };
    ~TinyDir() {
        tinydir_close(&dir);
    }

    std::vector<std::string> fileList() {
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

    tinydir_dir dir;
};
