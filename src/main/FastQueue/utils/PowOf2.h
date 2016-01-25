
#ifndef FASTQUEUE_UTILS_POWOF2_H
#define FASTQUEUE_UTILS_POWOF2_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "FastQueue/basic/stdsize.h"
#include "FastQueue/basic/stdint.h"

/* The implement way about power of 2, you can choose 1, 2 or 3. */
#define UTILS_POWOF2_MODE   1

namespace FastQueue {

namespace detail {

enum boolean_value_t {
    false_value = false,
    true_value = true
};

struct false_type {
    typedef false_type type;
    enum { value = false_value };
};

struct true_type {
    typedef true_type type;
    enum { value = true_value };
};

template<bool condition, class T, class U>
struct condition_if {
    typedef U type;
};

template <class T, class U>
struct condition_if<true, T, U> {
    typedef T type;
};

template<bool condition>
struct boolean_if {
    typedef typename condition_if<condition, true_type, false_type>::type type;
    enum { value = type::value };
};

} // namespace detail

namespace runtime {

template <typename SizeType>
inline bool is_pow2(SizeType n) {
    typedef std::make_unsigned<SizeType>::type UnsignedSizeType;
    UnsignedSizeType x = n;
    return ((x & (x - 1)) == 0);
}

template <typename SizeType>
inline SizeType verify_pow2(SizeType n) {
    typedef std::make_unsigned<SizeType>::type UnsignedSizeType;
    UnsignedSizeType x = n;
    return (x & (x - 1));
}

template <typename SizeType>
inline SizeType round_to_pow2(SizeType n) {
    st
    typedef std::make_unsigned<SizeType>::type UnsignedSizeType;
    UnsignedSizeType x;
    if (is_pow2(n)) {
        return n;
    }
    else {
        x = static_cast<UnsignedSizeType>(n - 1);
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        return static_cast<SizeType>(x + 1);
    }
}

template <>
inline uint64_t round_to_pow2(uint64_t n) {
    uint64_t x;
    if (is_pow2(n)) {
        return n;
    }
    else {
        x = n - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        x = x | (x >> 32);
        return (x + 1);
    }
}

template <>
inline int64_t round_to_pow2(int64_t n) {
    uint64_t x;
    if (is_pow2(n)) {
        return n;
    }
    else {
        x = static_cast<uint64_t>(n - 1);
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        x = x | (x >> 32);
        return static_cast<int64_t>(x + 1);
    }
}

} // namespace runtime

namespace static_const {

#if (UTILS_POWOF2_MODE <= 1) || !defined(UTILS_POWOF2_MODE)

//
// is_pow_of_2 = (N && ((N & (N - 1)) == 0);  // here, N must is unsigned number
//
template <size_t N>
struct is_pow2 {
    enum { value = detail::boolean_if<((N & (N - 1)) == 0)>::value };
};

template <>
struct is_pow2<0> {
    enum { value = detail::true_value };
};

//
// round_to_pow2<N> and next_is_pow2<N>
//
template <size_t N>
struct next_is_pow2 {
    enum { value = detail::boolean_if<((N & (N + 1)) == 0)>::value };
};

template <bool IsPow2, size_t N>
struct round_to_pow2_impl {
    enum {
        isPow2 = is_pow2<N>::value,
        nextIsPow2 = next_is_pow2<N>::value
    };
    static const size_t value = (isPow2 == 1) ? N : round_to_pow2_impl<(nextIsPow2 == detail::true_value), N + 1>::value;
};

template <size_t N>
struct round_to_pow2_impl<true, N> {
    static const size_t value = N;
};

template <size_t N>
struct round_to_pow2 {
    static const size_t value = round_to_pow2_impl<(is_pow2<N>::value == detail::true_value), N>::value;
};

//
// round_down_to_pow2<N> and front_is_pow2<N>
//
template <size_t N>
struct front_is_pow2 {
    enum { value = detail::boolean_if<(((N - 1) & (N - 2)) == 0)>::value };
};

template <bool IsPow2, size_t N>
struct round_down_to_pow2_impl {
    enum {
        isPow2 = is_pow2<N>::value,
        frontIsPow2 = front_is_pow2<N>::value
    };
    static const size_t value = ((isPow2 == detail::true_value) ? N : round_down_to_pow2_impl<(frontIsPow2 == detail::true_value), N - 1>::value);
};

template <size_t N>
struct round_down_to_pow2_impl<true, N> {
    static const size_t value = N;
};

template <size_t N>
struct round_down_to_pow2 {
    static const size_t value = round_down_to_pow2_impl<(is_pow2<N>::value == detail::true_value), N>::value;
};

#elif defined(UTILS_POWOF2_MODE) && (UTILS_POWOF2_MODE == 2)

//
// is_pow_of_2 = (N && ((N & (N - 1)) == 0);  // here, N must is unsigned number
//
template <size_t N>
struct is_pow2 {
    enum { value = detail::boolean_if<((N & (N - 1)) == 0)>::value };
};

template <>
struct is_pow2<0> {
    enum { value = detail::true_value };
};

template <size_t N>
struct round_to_pow2 {
    static const size_t value = ((is_pow2<N>::value == detail::true_value) ? N : round_to_pow2<N + 1>::value);
};

#define ROUND_TO_POW2_DEFINE(N)         \
template <>                             \
struct round_to_pow2<(N)> {             \
    static const size_t value = (N);    \
};

ROUND_TO_POW2_DEFINE(1 << 0)
ROUND_TO_POW2_DEFINE(1 << 1)
ROUND_TO_POW2_DEFINE(1 << 2)
ROUND_TO_POW2_DEFINE(1 << 3)
ROUND_TO_POW2_DEFINE(1 << 4)
ROUND_TO_POW2_DEFINE(1 << 5)
ROUND_TO_POW2_DEFINE(1 << 6)
ROUND_TO_POW2_DEFINE(1 << 7)
ROUND_TO_POW2_DEFINE(1 << 8)
ROUND_TO_POW2_DEFINE(1 << 9)
ROUND_TO_POW2_DEFINE(1 << 10)
ROUND_TO_POW2_DEFINE(1 << 11)
ROUND_TO_POW2_DEFINE(1 << 12)
ROUND_TO_POW2_DEFINE(1 << 13)
ROUND_TO_POW2_DEFINE(1 << 14)
ROUND_TO_POW2_DEFINE(1 << 15)
ROUND_TO_POW2_DEFINE(1 << 16)
ROUND_TO_POW2_DEFINE(1 << 17)
ROUND_TO_POW2_DEFINE(1 << 18)
ROUND_TO_POW2_DEFINE(1 << 19)
ROUND_TO_POW2_DEFINE(1 << 20)
ROUND_TO_POW2_DEFINE(1 << 21)
ROUND_TO_POW2_DEFINE(1 << 22)
ROUND_TO_POW2_DEFINE(1 << 23)
ROUND_TO_POW2_DEFINE(1 << 24)
ROUND_TO_POW2_DEFINE(1 << 25)
ROUND_TO_POW2_DEFINE(1 << 26)
ROUND_TO_POW2_DEFINE(1 << 27)
ROUND_TO_POW2_DEFINE(1 << 28)
ROUND_TO_POW2_DEFINE(1 << 29)
ROUND_TO_POW2_DEFINE(1 << 30)
ROUND_TO_POW2_DEFINE(1 << 31)

#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || defined(__amd64__) || defined(__x86_64__)
ROUND_TO_POW2_DEFINE(1ULL << 32)
ROUND_TO_POW2_DEFINE(1ULL << 33)
ROUND_TO_POW2_DEFINE(1ULL << 34)
ROUND_TO_POW2_DEFINE(1ULL << 35)
ROUND_TO_POW2_DEFINE(1ULL << 36)
ROUND_TO_POW2_DEFINE(1ULL << 37)
ROUND_TO_POW2_DEFINE(1ULL << 38)
ROUND_TO_POW2_DEFINE(1ULL << 39)
ROUND_TO_POW2_DEFINE(1ULL << 40)
ROUND_TO_POW2_DEFINE(1ULL << 41)
ROUND_TO_POW2_DEFINE(1ULL << 42)
ROUND_TO_POW2_DEFINE(1ULL << 43)
ROUND_TO_POW2_DEFINE(1ULL << 44)
ROUND_TO_POW2_DEFINE(1ULL << 45)
ROUND_TO_POW2_DEFINE(1ULL << 46)
ROUND_TO_POW2_DEFINE(1ULL << 47)
ROUND_TO_POW2_DEFINE(1ULL << 48)
ROUND_TO_POW2_DEFINE(1ULL << 49)
ROUND_TO_POW2_DEFINE(1ULL << 50)
ROUND_TO_POW2_DEFINE(1ULL << 51)
ROUND_TO_POW2_DEFINE(1ULL << 52)
ROUND_TO_POW2_DEFINE(1ULL << 53)
ROUND_TO_POW2_DEFINE(1ULL << 54)
ROUND_TO_POW2_DEFINE(1ULL << 55)
ROUND_TO_POW2_DEFINE(1ULL << 56)
ROUND_TO_POW2_DEFINE(1ULL << 57)
ROUND_TO_POW2_DEFINE(1ULL << 58)
ROUND_TO_POW2_DEFINE(1ULL << 59)
ROUND_TO_POW2_DEFINE(1ULL << 60)
ROUND_TO_POW2_DEFINE(1ULL << 61)
ROUND_TO_POW2_DEFINE(1ULL << 62)
ROUND_TO_POW2_DEFINE(1ULL << 63)
#endif // _WIN64 || _M_X64 || _M_AMD64

#undef ROUND_TO_POW2_DEFINE

#elif defined(UTILS_POWOF2_MODE) && (UTILS_POWOF2_MODE == 3)

//
// is_pow_of_2 = (N && ((N & (N - 1)) == 0);  // here, N must is unsigned number
//
template <size_t N>
struct is_pow2 {
    enum { value = detail::boolean_if<((N & (N - 1)) == 0)>::value };
};

template <>
struct is_pow2<0> {
    enum { value = detail::true_value };
};

template <size_t N>
struct round_to_pow2 {
    enum { value = ((is_pow2<N>::value == detail::true_value) ? N : round_to_pow2<N + 1>::value) };
};

#define ROUND_TO_POW2_DEFINE(N) \
template <>                     \
struct round_to_pow2<(N)> {     \
    enum { value = (N) };       \
};

ROUND_TO_POW2_DEFINE(1 << 0)
ROUND_TO_POW2_DEFINE(1 << 1)
ROUND_TO_POW2_DEFINE(1 << 2)
ROUND_TO_POW2_DEFINE(1 << 3)
ROUND_TO_POW2_DEFINE(1 << 4)
ROUND_TO_POW2_DEFINE(1 << 5)
ROUND_TO_POW2_DEFINE(1 << 6)
ROUND_TO_POW2_DEFINE(1 << 7)
ROUND_TO_POW2_DEFINE(1 << 8)
ROUND_TO_POW2_DEFINE(1 << 9)
ROUND_TO_POW2_DEFINE(1 << 10)
ROUND_TO_POW2_DEFINE(1 << 11)
ROUND_TO_POW2_DEFINE(1 << 12)
ROUND_TO_POW2_DEFINE(1 << 13)
ROUND_TO_POW2_DEFINE(1 << 14)
ROUND_TO_POW2_DEFINE(1 << 15)
ROUND_TO_POW2_DEFINE(1 << 16)
ROUND_TO_POW2_DEFINE(1 << 17)
ROUND_TO_POW2_DEFINE(1 << 18)
ROUND_TO_POW2_DEFINE(1 << 19)
ROUND_TO_POW2_DEFINE(1 << 20)
ROUND_TO_POW2_DEFINE(1 << 21)
ROUND_TO_POW2_DEFINE(1 << 22)
ROUND_TO_POW2_DEFINE(1 << 23)
ROUND_TO_POW2_DEFINE(1 << 24)
ROUND_TO_POW2_DEFINE(1 << 25)
ROUND_TO_POW2_DEFINE(1 << 26)
ROUND_TO_POW2_DEFINE(1 << 27)
ROUND_TO_POW2_DEFINE(1 << 28)
ROUND_TO_POW2_DEFINE(1 << 29)
ROUND_TO_POW2_DEFINE(1 << 30)
ROUND_TO_POW2_DEFINE(1 << 31)

#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || defined(__amd64__) || defined(__x86_64__)
ROUND_TO_POW2_DEFINE(1ULL << 32)
ROUND_TO_POW2_DEFINE(1ULL << 33)
ROUND_TO_POW2_DEFINE(1ULL << 34)
ROUND_TO_POW2_DEFINE(1ULL << 35)
ROUND_TO_POW2_DEFINE(1ULL << 36)
ROUND_TO_POW2_DEFINE(1ULL << 37)
ROUND_TO_POW2_DEFINE(1ULL << 38)
ROUND_TO_POW2_DEFINE(1ULL << 39)
ROUND_TO_POW2_DEFINE(1ULL << 40)
ROUND_TO_POW2_DEFINE(1ULL << 41)
ROUND_TO_POW2_DEFINE(1ULL << 42)
ROUND_TO_POW2_DEFINE(1ULL << 43)
ROUND_TO_POW2_DEFINE(1ULL << 44)
ROUND_TO_POW2_DEFINE(1ULL << 45)
ROUND_TO_POW2_DEFINE(1ULL << 46)
ROUND_TO_POW2_DEFINE(1ULL << 47)
ROUND_TO_POW2_DEFINE(1ULL << 48)
ROUND_TO_POW2_DEFINE(1ULL << 49)
ROUND_TO_POW2_DEFINE(1ULL << 50)
ROUND_TO_POW2_DEFINE(1ULL << 51)
ROUND_TO_POW2_DEFINE(1ULL << 52)
ROUND_TO_POW2_DEFINE(1ULL << 53)
ROUND_TO_POW2_DEFINE(1ULL << 54)
ROUND_TO_POW2_DEFINE(1ULL << 55)
ROUND_TO_POW2_DEFINE(1ULL << 56)
ROUND_TO_POW2_DEFINE(1ULL << 57)
ROUND_TO_POW2_DEFINE(1ULL << 58)
ROUND_TO_POW2_DEFINE(1ULL << 59)
ROUND_TO_POW2_DEFINE(1ULL << 60)
ROUND_TO_POW2_DEFINE(1ULL << 61)
ROUND_TO_POW2_DEFINE(1ULL << 62)
ROUND_TO_POW2_DEFINE(1ULL << 63)
#endif // _WIN64 || _M_X64 || _M_AMD64

#undef ROUND_TO_POW2_DEFINE

#else

static_assert("Error: The UTILS_POWOF2_MODE is more than 3.")

#endif // UTILS_POWOF2_MODE

#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || defined(__amd64__) || defined(__x86_64__)

template <>
struct round_to_pow2<18446744073709551615> {
    enum { value = detail::false_value };
};

#else

template <>
struct round_to_pow2<4294967295UL> {
    enum { value = detail::false_value };
};

#endif // _WIN64 || _M_X64 || _M_AMD64

} // namespace static_const

} // namespace TiActor

#endif  /* FASTQUEUE_UTILS_POWOF2_H */
