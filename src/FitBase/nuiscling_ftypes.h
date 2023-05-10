#ifndef NUISCLING_TYPES_H
#define NUISCLING_TYPES_H

#include "FitEvent.h"

namespace nuiscling_ftypes {
typedef bool (*filter)(FitEvent const *);
typedef double (*project)(FitEvent const *);
typedef double (*weight)(FitEvent const *);
} // namespace nuiscling_types

#endif