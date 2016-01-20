
#ifndef FASTQUEUE_BASIC_THREADLOCAL_H
#define FASTQUEUE_BASIC_THREADLOCAL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_MSC_VER)
  #define TI_THREAD_LOCAL    __declspec(thread)
#elif defined(__GNUC__)
  #define TI_THREAD_LOCAL   __thread
#elif defined(CXX11_HAVE_THREAD_LOCAL_DECLEAR)
  #define TI_THREAD_LOCAL   thread_local
#else
  #define TI_THREAD_LOCAL
#endif

#endif  /* FASTQUEUE_BASIC_THREAD_LOCAL_H */
