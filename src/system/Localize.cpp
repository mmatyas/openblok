#include "Localize.h"

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
