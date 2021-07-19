#include "Localize.h"

#include "Log.h"

#ifdef WITH_LOCALES
#include "Paths.h"
#include "libintl.h"
#endif // WITH_LOCALES


const char* const LOG_TAG = "locale";


#ifdef WITH_LOCALES

char* tr(const char* val)
{
    return gettext(val);
}

bool load_locale()
{
    const char* const locale = setlocale(LC_ALL, "");
    if (!locale) {
        Log::warning(LOG_TAG) << "Could not initialize localizations for the current locale\n";
        return false;
    }

    Log::info(LOG_TAG) << "Locale set to " << locale << '\n';

    const std::string locale_dir = Paths::data() + "locale";
    Log::info(LOG_TAG) << "Looking for locale files in " << locale_dir << "\n";

    const char* const DOMAIN = "main";
    bindtextdomain(DOMAIN, locale_dir.c_str());
    textdomain(DOMAIN);

    return true;
}

#else // WITH_LOCALES

bool load_locale()
{
    Log::info(LOG_TAG) << "Built without localization support\n";
    return true;
}

#endif // WITH_LOCALES
