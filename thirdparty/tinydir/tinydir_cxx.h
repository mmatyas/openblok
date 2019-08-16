#pragma once

#include <tinydir.h>

#include <string>
#include <vector>


#ifdef _MSC_VER
#  define WIN32_LEAN_AND_MEAN
#  include <io.h>
#  define posix_access _access
#else
#  include <unistd.h>
#  define posix_access access
#endif

#if defined _MSC_VER || defined __MINGW32__
#  undef INFINITE
#endif


inline bool path_exists(const std::string& path) {
    return posix_access(path.c_str(), 0) == 0;
}


class TinyDir {
public:
    TinyDir(const std::string& path);
    ~TinyDir();

    std::vector<std::string> file_list();

    tinydir_dir dir;
};
