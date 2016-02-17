
#ifndef FASTQUEUE_BASIC_STDDEF_H
#define FASTQUEUE_BASIC_STDDEF_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "FastQueue/basic/stdint.h"

#if defined(FASTQUEUE_USE_SHARED)
#define FASTQUEUE_API        extern "C" _declspec(dllimport)
#elif defined(FASTQUEUE_SHARED_DLL)
#define FASTQUEUE_API        extern "C" _declspec(dllexport)
#else
// FASTQUEUE_USE_STATIC, FASTQUEUE_STATIC_LIB
#define FASTQUEUE_API        extern "C" static
#endif

#ifndef TI_CACHELINE_SIZE
#define TI_CACHELINE_SIZE    64
#endif

#endif  /* FASTQUEUE_BASIC_STDDEF_H */
