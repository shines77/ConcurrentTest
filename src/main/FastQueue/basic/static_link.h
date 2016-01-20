
#ifndef FASTQUEUE_BASIC_STATIC_LINK_H
#define FASTQUEUE_BASIC_STATIC_LINK_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_WIN64) || defined(WIN64)
#  if defined(_DEBUG) && !defined(NDEBUG)
#    pragma comment(lib, "libFastQueue-x64-Debug.lib")
#  else
#    pragma comment(lib, "libFastQueue-x64-Release.lib")
#  endif
#elif defined(_WIN32) || defined(WIN32) || defined(_WINDOWS)
#  if defined(_DEBUG) && !defined(NDEBUG)
#    pragma comment(lib, "libFastQueue-x86-Debug.lib")
#  else
#    pragma comment(lib, "libFastQueue-x86-Release.lib")
#  endif
#else
// Linux or another OS
// Do nothing!!
#endif

#endif  /* FASTQUEUE_BASIC_STATIC_LINK_H */
