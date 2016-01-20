
#ifndef FASTQUEUE_BASIC_STDBOOL_H
#define FASTQUEUE_BASIC_STDBOOL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1700)
#include "FastQueue/basic/msvc/stdbool.h"
#else
#include <stdbool.h>
#endif  /* _MSC_VER */

#endif  /* FASTQUEUE_BASIC_STDBOOL_H */
