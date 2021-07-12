#pragma once

// Translatable texts should be prefixed with `tr`
#ifdef WITH_LOCALES

#include "libintl.h"
#define tr(val) gettext(val)

#else // WITH_LOCALES

#define tr(val) val

#endif // WITH_LOCALES

bool load_locale();
