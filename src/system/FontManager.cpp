#include "FontManager.h"

#include "fontconfig/FontConfig.h"


std::unique_ptr<FontManager> FontManager::create()
{
    return std::make_unique<FontConfig>();
}
