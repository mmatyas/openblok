#include "Paths.h"

#include "SDL2/SDL_filesystem.h" // TODO: fix this


#ifndef OPENBLOK_DATADIR
#define OPENBLOK_DATADIR "./data"
#endif

std::string defaultDataDir()
{
    std::string path(OPENBLOK_DATADIR);
    if (path.front() == '.')
        path = SDL_GetBasePath() + path;

    return path + '/';
}

std::string Paths::datadir_path = defaultDataDir();

const std::string Paths::configdir_path = SDL_GetPrefPath(".", "openblok");

void Paths::changeDataDir(const std::string& dir)
{
    datadir_path = dir;
    if (datadir_path.back() != '/' && datadir_path.back() != '\\')
        datadir_path += '/';
}
