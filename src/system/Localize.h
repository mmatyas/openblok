#pragma once

// Translatable texts should be prefixed with `tr`
#include "libintl.h"
#define tr(val) gettext(val)
