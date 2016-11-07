#include "Paths.h"

#include "SDL2/SDL_filesystem.h" // TODO: fix this


#ifndef OPENBLOK_DATADIR
#define OPENBLOK_DATADIR "data"
#endif
std::string Paths::datadir_path = std::string(OPENBLOK_DATADIR) + "/";

const std::string Paths::configdir_path = SDL_GetPrefPath(".", "openblok");

void Paths::changeDataDir(const std::string& dir)
{
    datadir_path = dir;
    if (datadir_path.back() != '/' && datadir_path.back() != '\\')
        datadir_path += '/';
}
