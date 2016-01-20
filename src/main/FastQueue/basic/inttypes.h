
#ifndef FASTQUEUE_BASIC_INTTYPES_H
#define FASTQUEUE_BASIC_INTTYPES_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1700)
#include "FastQueue/basic/msvc/inttypes.h"
#else
#include <inttypes.h>
#endif  /* _MSC_VER */

#endif  /* FASTQUEUE_INTTYPES_H */
