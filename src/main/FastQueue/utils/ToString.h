
#ifndef FASTQUEUE_UTILS_TOSTRING_H
#define FASTQUEUE_UTILS_TOSTRING_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>

//
// See: http://stackoverflow.com/questions/24975147/check-if-class-has-function-with-signature
//

#undef  DEFINE_METHOD_CHECKER
#define DEFINE_METHOD_CHECKER(RETURN_TYPE, METHOD_NAME, PARAMETERS)         \
template<typename T>                                                        \
class Check ## METHOD_NAME ## FunctionIsExists                              \
{                                                                           \
private:                                                                    \
    typedef char true_type;                                                 \
    typedef char (&false_type)[2];                                          \
                                                                            \
    template <typename U, RETURN_TYPE (U::*)PARAMETERS = &U::METHOD_NAME>   \
    struct checker {                                                        \
        typedef true_type type;                                             \
    };                                                                      \
                                                                            \
    template <typename U>                                                   \
    static typename checker<U>::type tester(const U *);                     \
                                                                            \
    static false_type tester(...);                                          \
                                                                            \
public:                                                                     \
    enum { value = (sizeof(tester(static_cast<const T *>(0))) == sizeof(true_type)) }; \
}

// ChecktoStringFunctionIsExists<T>::value, std::string toString(void) const
// DEFINE_METHOD_CHECKER(std::string, toString, (void) const);

namespace FastQueue {

namespace StringUtils {

//
// See: http://blog.chinaunix.net/uid-1720597-id-306773.html
//

namespace detail {

// ChecktoStringFunctionIsExists<T>::value, std::string toString(void) const
DEFINE_METHOD_CHECKER(std::string, toString, (void) const);

template<typename T>
struct HasToStringFunction {
    template<typename U, std::string(U::*)() const>
    struct matcher;

    template<typename U>
    static char helper(matcher<U, &U::toString> *);

    template<typename U>
    static int helper(...);

    enum { value = (sizeof(helper<T>(nullptr)) == sizeof(char)) };
};

template <bool>
struct ToStringWrapper {};

template<>
struct ToStringWrapper<true> {
    template<typename T>
    static std::string toString(const T & x) {
        return x.toString();
    }
};

template<>
struct ToStringWrapper<false> {
    template<typename T>
    static std::string toString(const T & x) {
        return std::string(typeid(x).name());
    }
};

} // namespace detail */

template<typename T>
std::string toString(const T & x) {
    return detail::ToStringWrapper<detail::ChecktoStringFunctionIsExists<T>::value>::toString(x);
}

template<typename T>
std::string toString2(const T & x) {
    return detail::ToStringWrapper<detail::HasToStringFunction<T>::value>::toString(x);
}

} // namespace StringUtils

} // namespace TiActor

#endif  /* FASTQUEUE_UTILS_TOSTRING_H */
