
#ifndef FASTQUEUE_UTILS_STRINGUTILS_H
#define FASTQUEUE_UTILS_STRINGUTILS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>

#include "FastQueue/basic/stdsize.h"
#include "FastQueue/basic/threadlocal.h"

namespace FastQueue {
namespace StringUtils {

static TI_THREAD_LOCAL std::string * tlsFormatStr = nullptr;
//static thread_local std::string * tlsFormatStr2 = nullptr;

static
std::size_t
vsnprintf(char * destBuffer, size_t sizeInBytes, size_t maxCount,
          const char * format, va_list argList) {
    //
    return 0;
}

static
const char *
format(std::size_t bufsize, const char * format, ...) {
    // Added a null char, buffer size minium is 64 bytes.
    std::size_t maxcount = (bufsize > 64) ? (bufsize + 1) : 64;
    if (tlsFormatStr == nullptr) {
        tlsFormatStr = new std::string("");
    }
    tlsFormatStr->reserve(maxcount);
    //tlsFormatStr->resize(maxcount);

    va_list arg_list;
    va_start(arg_list, format);
    char * buffer = (char *)tlsFormatStr->c_str();
    int ret_len = _vsnprintf_s(buffer, maxcount, maxcount - 1, format, arg_list);
    va_end(arg_list);
    return tlsFormatStr->c_str();
}

static
const char *
format(char * buffer, std::size_t size_in_bytes,
       std::size_t max_count, const char * format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    int ret_len = _vsnprintf_s(buffer, size_in_bytes, max_count, format, arg_list);
    va_end(arg_list);
    return buffer;
}

} // namespace StringUtils
} // namespace FastQueue

#endif  /* FASTQUEUE_UTILS_STRINGUTILS_H */
