#include "Paths.h"

#include "SDL2/SDL_filesystem.h" // TODO: fix this


#ifndef OPENBLOK_DATADIR
#define OPENBLOK_DATADIR "./data"
#endif

std::string defaultDataDir()
{
    std::string path(OPENBLOK_DATADIR);
    if (path.front() == '.') {
        const auto basepath_raw = SDL_GetBasePath();
        path.insert(0, basepath_raw);
        SDL_free(basepath_raw);
    }
    return path + '/';
}

std::string defaultConfigDir()
{
    const auto path_raw = SDL_GetPrefPath(".", "openblok");
    const std::string path(path_raw);
    SDL_free(path_raw);
    return path;
}

std::string Paths::datadir_path = defaultDataDir();
const std::string Paths::configdir_path = defaultConfigDir();

void Paths::changeDataDir(const std::string& dir)
{
    datadir_path = dir;
    if (datadir_path.back() != '/' && datadir_path.back() != '\\')
        datadir_path += '/';
}
