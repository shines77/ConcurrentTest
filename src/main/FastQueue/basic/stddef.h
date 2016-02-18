
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

//#include <stdalign.h>
//#include <cstdalign>

#if defined(_MSC_VER) || defined(__INTER_COMPILER) || defined(__ICC)

#define ALIGN_PREFIX(N)         __declspec(align(N))
#define ALIGN_SUFFIX(N)

//#define CACHE_ALIGN_PREFIX    __declspec(align(TI_CACHELINE_SIZE))
#define CACHE_ALIGN_PREFIX      alignas(TI_CACHELINE_SIZE)
#define CACHE_ALIGN_SUFFIX

#define PACKED_ALIGN_PREFIX(N)  __declspec(align(N))
#define PACKED_ALIGN_SUFFIX(N)

#else

#define ALIGN_PREFIX(N)         __attribute__((__aligned__((N))))
#define ALIGN_SUFFIX(N)

#define CACHE_ALIGN_PREFIX      __attribute__((__aligned__((TI_CACHELINE_SIZE))))
#define CACHE_ALIGN_SUFFIX

#define PACKED_ALIGN_PREFIX(N)
#define PACKED_ALIGN_SUFFIX(N)  __attribute__((packed, aligned(N)))

#endif

#endif  /* FASTQUEUE_BASIC_STDDEF_H */
