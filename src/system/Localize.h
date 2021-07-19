#pragma once

// Translatable texts should be prefixed with `tr`
#ifdef WITH_LOCALES
char* tr(const char* val);
#else
#define tr(val) val
#endif

bool load_locale();
