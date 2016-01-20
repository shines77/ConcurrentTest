
#ifndef FASTQUEUE_UTILS_POWOF2_H
#define FASTQUEUE_UTILS_POWOF2_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "FastQueue/basic/stdsize.h"

namespace FastQueue {

namespace math {

struct false_type {
    typedef false_type type;
    enum { value = 0 };
};

struct true_type {
    typedef true_type type;
    enum { value = 1 };
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

//
// is_pow_of_2 = (N && ((N & (N - 1)) == 0);  // here, N must is unsigned number
//
template <size_t N>
struct is_pow_of_2 {
    enum { value = boolean_if<((N & (N - 1)) == 0)>::value };
};

template <>
struct is_pow_of_2<0> {
    enum { value = 1 };
};

#if 1

#define ROUND_UP_TO_POW2_DEF(N)

template <size_t N>
struct next_is_pow_of_2 {
    enum { value = boolean_if<((N & (N + 1)) == 0)>::value };
};

template <bool is_pow2, size_t N>
struct round_up_to_pow2_impl {
    enum {
        isPow2 = is_pow_of_2<N>::value,
        nextIsPow2 = next_is_pow_of_2<N>::value
    };
    static const size_t value = (isPow2 == 1) ? N : round_up_to_pow2_impl<(nextIsPow2 == 1), N + 1>::value;
};

template <size_t N>
struct round_up_to_pow2_impl<true, N> {
    static const size_t value = N;
};

template <size_t N>
struct round_up_to_pow2 {
    static const size_t value = round_up_to_pow2_impl<(is_pow_of_2<N>::value == 1), N>::value;
};

#elif 1

#define ROUND_UP_TO_POW2_DEF(N)         \
template <>                             \
struct round_up_to_pow2<(N)> {          \
    static const size_t value = (N);    \
};

template <size_t N>
struct round_up_to_pow2 {
    static const size_t value = (is_pow_of_2<N>::value == 1) ? N : round_up_to_pow2<N + 1>::value;
};

#else

#define ROUND_UP_TO_POW2_DEF(N) \
template <>                     \
struct round_up_to_pow2<(N)> {  \
    enum { value = (N) };       \
};

template <size_t N>
struct round_up_to_pow2 {
    enum { value = (is_pow_of_2<N>::value == 1) ? N : round_up_to_pow2<N + 1>::value };
};

#endif

ROUND_UP_TO_POW2_DEF(1 << 0)
ROUND_UP_TO_POW2_DEF(1 << 1)
ROUND_UP_TO_POW2_DEF(1 << 2)
ROUND_UP_TO_POW2_DEF(1 << 3)
ROUND_UP_TO_POW2_DEF(1 << 4)
ROUND_UP_TO_POW2_DEF(1 << 5)
ROUND_UP_TO_POW2_DEF(1 << 6)
ROUND_UP_TO_POW2_DEF(1 << 7)
ROUND_UP_TO_POW2_DEF(1 << 8)
ROUND_UP_TO_POW2_DEF(1 << 9)
ROUND_UP_TO_POW2_DEF(1 << 10)
ROUND_UP_TO_POW2_DEF(1 << 11)
ROUND_UP_TO_POW2_DEF(1 << 12)
ROUND_UP_TO_POW2_DEF(1 << 13)
ROUND_UP_TO_POW2_DEF(1 << 14)
ROUND_UP_TO_POW2_DEF(1 << 15)
ROUND_UP_TO_POW2_DEF(1 << 16)
ROUND_UP_TO_POW2_DEF(1 << 17)
ROUND_UP_TO_POW2_DEF(1 << 18)
ROUND_UP_TO_POW2_DEF(1 << 19)
ROUND_UP_TO_POW2_DEF(1 << 20)
ROUND_UP_TO_POW2_DEF(1 << 21)
ROUND_UP_TO_POW2_DEF(1 << 22)
ROUND_UP_TO_POW2_DEF(1 << 23)
ROUND_UP_TO_POW2_DEF(1 << 24)
ROUND_UP_TO_POW2_DEF(1 << 25)
ROUND_UP_TO_POW2_DEF(1 << 26)
ROUND_UP_TO_POW2_DEF(1 << 27)
ROUND_UP_TO_POW2_DEF(1 << 28)
ROUND_UP_TO_POW2_DEF(1 << 29)
ROUND_UP_TO_POW2_DEF(1 << 30)
ROUND_UP_TO_POW2_DEF(1 << 31)

#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || defined(__amd64__) || defined(__x86_64__)
ROUND_UP_TO_POW2_DEF(1ULL << 32)
ROUND_UP_TO_POW2_DEF(1ULL << 33)
ROUND_UP_TO_POW2_DEF(1ULL << 34)
ROUND_UP_TO_POW2_DEF(1ULL << 35)
ROUND_UP_TO_POW2_DEF(1ULL << 36)
ROUND_UP_TO_POW2_DEF(1ULL << 37)
ROUND_UP_TO_POW2_DEF(1ULL << 38)
ROUND_UP_TO_POW2_DEF(1ULL << 39)
ROUND_UP_TO_POW2_DEF(1ULL << 40)
ROUND_UP_TO_POW2_DEF(1ULL << 41)
ROUND_UP_TO_POW2_DEF(1ULL << 42)
ROUND_UP_TO_POW2_DEF(1ULL << 43)
ROUND_UP_TO_POW2_DEF(1ULL << 44)
ROUND_UP_TO_POW2_DEF(1ULL << 45)
ROUND_UP_TO_POW2_DEF(1ULL << 46)
ROUND_UP_TO_POW2_DEF(1ULL << 47)
ROUND_UP_TO_POW2_DEF(1ULL << 48)
ROUND_UP_TO_POW2_DEF(1ULL << 49)
ROUND_UP_TO_POW2_DEF(1ULL << 50)
ROUND_UP_TO_POW2_DEF(1ULL << 51)
ROUND_UP_TO_POW2_DEF(1ULL << 52)
ROUND_UP_TO_POW2_DEF(1ULL << 53)
ROUND_UP_TO_POW2_DEF(1ULL << 54)
ROUND_UP_TO_POW2_DEF(1ULL << 55)
ROUND_UP_TO_POW2_DEF(1ULL << 56)
ROUND_UP_TO_POW2_DEF(1ULL << 57)
ROUND_UP_TO_POW2_DEF(1ULL << 58)
ROUND_UP_TO_POW2_DEF(1ULL << 59)
ROUND_UP_TO_POW2_DEF(1ULL << 60)
ROUND_UP_TO_POW2_DEF(1ULL << 61)
ROUND_UP_TO_POW2_DEF(1ULL << 62)
ROUND_UP_TO_POW2_DEF(1ULL << 63)
#endif

#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || defined(__amd64__) || defined(__x86_64__)
template <>
struct round_up_to_pow2<18446744073709551615> {
    enum { value = 0 };
};
#else
template <>
struct round_up_to_pow2<4294967295UL> {
    enum { value = 0 };
};
#endif

#undef ROUND_UP_TO_POW2_DEF

template <size_t N>
struct front_is_pow_of_2 {
    enum { value = boolean_if<(((N - 1) & (N - 2)) == 0)>::value };
};

template <bool is_pow2, size_t N>
struct round_down_to_pow2_impl {
    enum {
        isPow2 = is_pow_of_2<N>::value,
        frontIsPow2 = front_is_pow_of_2<N>::value
    };
    static const size_t value = (isPow2 == 1) ? N : round_down_to_pow2_impl<(frontIsPow2 == 1), N - 1>::value;
};

template <size_t N>
struct round_down_to_pow2_impl<true, N> {
    static const size_t value = N;
};

template <size_t N>
struct round_down_to_pow2 {
    static const size_t value = round_down_to_pow2_impl<(is_pow_of_2<N>::value == 1), N>::value;
};

} // namespace math

} // namespace TiActor

#endif  /* FASTQUEUE_UTILS_POWOF2_H */
