
#ifndef FASTQUEUE_UTILS_RUNTIME_H
#define FASTQUEUE_UTILS_RUNTIME_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)  || defined(__ICC) \
    || defined(__MINGW32__) || defined(__CYGWIN__) || defined(__MSYS__)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#include <windows.h>
#elif defined(__linux__) || defined(__GUNC__) \
    || defined(__clang__) || defined(__APPLE__) || defined(__FreeBSD__) \
    || defined(LINUX) || defined(SOLARIS) || defined(AIX)
#include <sys/sysinfo.h>    // For get_nprocs() & get_nprocs_conf()
#include <unistd.h>         // For sysconf()
#endif

namespace FastQueue {

namespace Runtime {

/* 
 * - _SC_NPROCESSORS_CONF
 *       The number of processors configured.
 * 
 * - _SC_NPROCESSORS_ONLN
 *       The number of processors currently online (available).
 */

static
int getAvailableProcessors() {
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)  || defined(__ICC) \
    || defined(__MINGW32__) || defined(__CYGWIN__) || defined(__MSYS__)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
#elif defined(__linux__) || defined(__GUNC__) \
    || defined(__clang__) || defined(__APPLE__) || defined(__FreeBSD__) \
    || defined(LINUX) || defined(SOLARIS) || defined(AIX)
    int nprocs = -1;
  #ifdef _SC_NPROCESSORS_ONLN
    nprocs = sysconf(_SC_NPROCESSORS_ONLN);
  #else
    nprocs = get_nprocs();    // GNU fuction
  #endif
    return nprocs;
#else
    return 1;
#endif
}

static
int getPhysicalProcessors() {
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)  || defined(__ICC) \
    || defined(__MINGW32__) || defined(__CYGWIN__) || defined(__MSYS__)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
#elif defined(__linux__) || defined(__GUNC__) \
    || defined(__clang__) || defined(__APPLE__) || defined(__FreeBSD__) \
    || defined(LINUX) || defined(SOLARIS) || defined(AIX)
    int nprocs = -1;
  #ifdef _SC_NPROCESSORS_CONF
    nprocs = sysconf(_SC_NPROCESSORS_CONF);
  #else
    nprocs = get_nprocs_conf();     // GNU fuction
  #endif
    return nprocs;
#else
    return 1;
#endif
}

} // namespace Runtime

} // namespace TiActor

#endif  /* FASTQUEUE_UTILS_RUNTIME_H */
