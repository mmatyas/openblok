#include "Localize.h"

#ifdef WITH_LOCALES

#include "Paths.h"

#include <string>


bool load_locale()
{
    const char* const locale = setlocale(LC_ALL, "");
    if (!locale)
        return false;

    const std::string locale_dir = Paths::data() + "locale";
    bindtextdomain("main", locale_dir.c_str());
    textdomain("main");

    return true;
}

#else // WITH_LOCALES

bool load_locale()
{
    return true;
}

#endif // WITH_LOCALES
