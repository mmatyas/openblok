#include "Paths.h"

#ifndef OPENBLOK_DATADIR
#define OPENBLOK_DATADIR "data"
#endif
const std::string Paths::datadir_path = std::string(OPENBLOK_DATADIR) + "/";
