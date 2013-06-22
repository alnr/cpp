/*
 *    Copyright (c) 2013 Arne Luenser
 */

#ifndef EC7129FF06F0B6C498A0D1F0974BA0C4133674A81278D430F4ABEE9ADCA1F701
#define EC7129FF06F0B6C498A0D1F0974BA0C4133674A81278D430F4ABEE9ADCA1F701

#include <cmath>
#include <cstdlib> // for std:abs<int>() and std::div<int>()
#include <utility>

namespace alnr
{

/*
 * Generic common mathematical function objects. These are designed in analogy
 * to the C++1y-style generic function objects in <functional>. Their default
 * template parameter (void) is ignored. Instead, operator() is templated.
 * This is mostly useful when using algorithms from <algorithm> or <numeric>
 * in combination with mathematical functions such as std::abs. Because those
 * operations (defined in <cmath>) are function templates, their use as 
 * function objects is cumbersome, as they must be explicitly casted to the
 * correct type.
 *
 * Usage:
 *    auto os = ostreamer<>(std::cout, ", ");
 *    std::vector<double> v {-1.1, -2.2, -3.3, -4.4};
 *    std::transform(v.begin(), v.end(), os, math::abs<>());
 *    // instead of
 *    // std::transform(v.begin(), v.end(), os,
 *    //                static_cast<double(*)(double)>(std::abs));
 *    // prints: "1.1, 2.2, 3.3, 4.4, "
 */
namespace math
{

#define ALNR_UNARY_FUNCTION_OBJECT(name) \
template <typename X = void> \
struct name { \
    template<typename T> \
    auto operator()(T&& t) const \
        -> decltype(std::name(std::forward<T>(t))) { \
        return std::name(std::forward<T>(t)); \
    } \
}

#define ALNR_BINARY_FUNCTION_OBJECT(name) \
template <typename X = void> \
struct name { \
    template<typename T, typename U> \
    auto operator()(T&& t, U&& u) const \
        -> decltype(std::name(std::forward<T>(t), std::forward<U>(u))) { \
        return std::name(std::forward<T>(t), std::forward<U>(u)); \
    } \
}

#define ALNR_TERNARY_FUNCTION_OBJECT(name) \
template <typename X = void> \
struct name { \
    template<typename T, typename U, typename V> \
    auto operator()(T&& t, U&& u, V&& v) const \
        -> decltype(std::name(std::forward<T>(t), \
                              std::forward<U>(u), \
                              std::forward<V>(v))) { \
        return std::name(std::forward<T>(t), \
                         std::forward<U>(u), \
                         std::forward<V>(v)); \
    } \
}

/*
 * Functions from <cmath>
 * (ISO/IEC 14882:2011 26.8.9)
 *
 * (1) Unary functions
 */
ALNR_UNARY_FUNCTION_OBJECT(abs);
ALNR_UNARY_FUNCTION_OBJECT(acos);
ALNR_UNARY_FUNCTION_OBJECT(acosh);
ALNR_UNARY_FUNCTION_OBJECT(asin);
ALNR_UNARY_FUNCTION_OBJECT(asinh);
ALNR_UNARY_FUNCTION_OBJECT(atan);
ALNR_UNARY_FUNCTION_OBJECT(atanh);
ALNR_UNARY_FUNCTION_OBJECT(cbrt);
ALNR_UNARY_FUNCTION_OBJECT(ceil);
ALNR_UNARY_FUNCTION_OBJECT(cos);
ALNR_UNARY_FUNCTION_OBJECT(cosh);
ALNR_UNARY_FUNCTION_OBJECT(erfc);
ALNR_UNARY_FUNCTION_OBJECT(erf);
ALNR_UNARY_FUNCTION_OBJECT(exp2);
ALNR_UNARY_FUNCTION_OBJECT(exp);
ALNR_UNARY_FUNCTION_OBJECT(expm1);
ALNR_UNARY_FUNCTION_OBJECT(fabs);
ALNR_UNARY_FUNCTION_OBJECT(floor);
ALNR_UNARY_FUNCTION_OBJECT(ilogb);
ALNR_UNARY_FUNCTION_OBJECT(lgamma);
ALNR_UNARY_FUNCTION_OBJECT(llrint);
ALNR_UNARY_FUNCTION_OBJECT(llround);
ALNR_UNARY_FUNCTION_OBJECT(log10);
ALNR_UNARY_FUNCTION_OBJECT(log1p);
ALNR_UNARY_FUNCTION_OBJECT(log2);
ALNR_UNARY_FUNCTION_OBJECT(logb);
ALNR_UNARY_FUNCTION_OBJECT(log);
ALNR_UNARY_FUNCTION_OBJECT(lrint);
ALNR_UNARY_FUNCTION_OBJECT(lround);
ALNR_UNARY_FUNCTION_OBJECT(nearbyint);
ALNR_UNARY_FUNCTION_OBJECT(rint);
ALNR_UNARY_FUNCTION_OBJECT(round);
ALNR_UNARY_FUNCTION_OBJECT(sin);
ALNR_UNARY_FUNCTION_OBJECT(sinh);
ALNR_UNARY_FUNCTION_OBJECT(sqrt);
ALNR_UNARY_FUNCTION_OBJECT(tan);
ALNR_UNARY_FUNCTION_OBJECT(tanh);
ALNR_UNARY_FUNCTION_OBJECT(tgamma);
ALNR_UNARY_FUNCTION_OBJECT(trunc);

/*
 * (2) Binary functions
 */
ALNR_BINARY_FUNCTION_OBJECT(atan2);
ALNR_BINARY_FUNCTION_OBJECT(copysign);
ALNR_BINARY_FUNCTION_OBJECT(fdim);
ALNR_BINARY_FUNCTION_OBJECT(fmax);
ALNR_BINARY_FUNCTION_OBJECT(fmin);
ALNR_BINARY_FUNCTION_OBJECT(fmod);
ALNR_BINARY_FUNCTION_OBJECT(frexp);
ALNR_BINARY_FUNCTION_OBJECT(hypot);
ALNR_BINARY_FUNCTION_OBJECT(ldexp);
ALNR_BINARY_FUNCTION_OBJECT(modf);
ALNR_BINARY_FUNCTION_OBJECT(nextafter);
ALNR_BINARY_FUNCTION_OBJECT(nexttoward);
ALNR_BINARY_FUNCTION_OBJECT(pow);
ALNR_BINARY_FUNCTION_OBJECT(remainder);
ALNR_BINARY_FUNCTION_OBJECT(scalbln);
ALNR_BINARY_FUNCTION_OBJECT(scalbn);

/*
 * (3) Ternary functions
 */
ALNR_TERNARY_FUNCTION_OBJECT(fma);
ALNR_TERNARY_FUNCTION_OBJECT(remquo);

/*
 * Functions from <cstdlib>
 * (ISO/IEC 14882:2011 26.8.7)
 *
 * (1) Unary functions
 */
// ALNR_UNARY_FUNCTION_OBJECT(abs); //already covered above

ALNR_BINARY_FUNCTION_OBJECT(div);

/*
 * Floating point classification/comparison functions from <cmath>
 * (ISO/IEC 14882:2011 26.8.10)
 *
 * (1) Unary functions
 */
ALNR_UNARY_FUNCTION_OBJECT(fpclassify);
ALNR_UNARY_FUNCTION_OBJECT(isfinite);
ALNR_UNARY_FUNCTION_OBJECT(isinf);
ALNR_UNARY_FUNCTION_OBJECT(isnan);
ALNR_UNARY_FUNCTION_OBJECT(isnormal);
ALNR_UNARY_FUNCTION_OBJECT(signbit);

/*
 * (2) Binary functions
 */
ALNR_BINARY_FUNCTION_OBJECT(isgreater);
ALNR_BINARY_FUNCTION_OBJECT(isgreaterequal);
ALNR_BINARY_FUNCTION_OBJECT(isless);
ALNR_BINARY_FUNCTION_OBJECT(islessequal);
ALNR_BINARY_FUNCTION_OBJECT(islessgreater);
ALNR_BINARY_FUNCTION_OBJECT(isunordered);

#undef ALNR_UNARY_FUNCTION_OBJECT
#undef ALNR_BINARY_FUNCTION_OBJECT
#undef ALNR_TERNARY_FUNCTION_OBJECT

} /* namespace math */

} /* namespace alnr */

#endif

