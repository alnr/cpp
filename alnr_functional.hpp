/*
 *    Copyright (c) 2013 Arne Luenser
 */

#ifndef F487DC3045F53649BE3584DDC8D06608771D375A9D2F7DDDE877FC02D20CCA92
#define F487DC3045F53649BE3584DDC8D06608771D375A9D2F7DDDE877FC02D20CCA92

#include <cstddef>
#include <cmath>
#include <tuple>
#include <utility>
#include <type_traits>
#include <iterator>

#define RETURNS_AS_IS(...) -> decltype(__VA_ARGS__) { return (__VA_ARGS__); }
#define RETURNS_DECAY(...) -> typename std::decay<decltype(__VA_ARGS__)>::type\
    { return (__VA_ARGS__); }

/*
 * C++1y-friendly adaptation of select1st and select2nd function objects
 * from SGI's original STL. These are valuable in generic functional
 * programming contexts. Works with anything where std::get() works
 * (e.g. std::pair, std::tuples, std::array).
 *
 * Usage (basic):
 *     typedef std::pair<int, double> pair_t;
 *     pair_t p = { -1, 42.3 };
 *     int i = select1st<>(p); // i == -1
 *     double d = select2nd<>(p) // d == 42.3
 *
 * Usage (advanced):
 *     typedef std::pair<std::string, std::string> pair_t;
 *     typedef std::map<int, pair_t> map_t;
 *     typedef std::vector<pair_t> vec_t;
 *     map_t m;
 *     m[1972] = pair_t("The Godfather", "Francis Ford Coppola");
 *     m[1982] = pair_t("Scarface", "Brian De Palma");
 *     m[1994] = pair_t("Pulp Fiction", "Quentin Tarantino");
 *     vec_t v;
 *     v.reserve(m.size());
 *     std::transform(m.begin(), m.end(), back_inserter(v), select2nd<>());
 *     // instead of:
 *     // std::transform(m.begin(), m.end(), back_inserter(v),
 *     //                [](const map_t::value_type& x){ return x.second; });
 */
template <typename T = void>
struct select1st
{
    template<typename P>
    auto operator()(P&& p) const
    RETURNS_AS_IS(
        std::get<0>(std::forward<P>(p))
    )
};

template <typename T = void>
struct select2nd
{
    template<typename P>
    auto operator()(P&& p) const
    RETURNS_AS_IS(
        std::get<1>(std::forward<P>(p))
    )
};

/*
 * C++1y-friendly adaptation of the identity, project1st and project2nd 
 * function objects from SGI's original STL. identity<> takes one argument and
 * returns it unmodified. project1st<> takes two arguments, and returns the
 * first unmodified. Analogously, project2nd<> takes two arguments, and returns
 * the second unmodified. Uses perfect forwarding.
 */
template <typename T = void>
struct identity
{
    template<typename T1>
    auto operator()(T1&& t1) const
    RETURNS_AS_IS(
        std::forward<T1>(t1)
    )
};

template <typename T = void>
struct project1st
{
    template<typename T1, typename T2>
    auto operator()(T1&& t1, T2&&) const
    RETURNS_AS_IS(
        std::forward<T1>(t1)
    )
};

template <typename T = void>
struct project2nd
{
    template<typename T1, typename T2>
    auto operator()(T1&&, T2&& t2) const
    RETURNS_AS_IS(
        std::forward<T2>(t2)
    )
};

/*
 * Higher-order function: composition
 *
 * compose(f, g, h, ...) returns the right-leaning composition of the 
 * functions f, g, h, ... compose(f,g,h)(x) thus evaluates to f(g(h(x))).
 * The first function to evaluate (i.e., the rightmost in the composition)
 * may take and arbitrary number of arguments, and return one value. All other
 * functions must take exactly one argument, and return one value. Arbitrary
 * argument and return types are allowed, provided the composition as a whole
 * has consistent typing (see example). The function arguments may be any
 * callable type, such as normal functions, function objects, and lambdas.
 * Note: overloaded functions require explicit casting to disambiguate
 *       (see first example).
 *
 * Usage (I):
 *     std::cout << compose(
 *         [](std::string s) { return s + "!"; },
 *         static_cast<std::string(*)(int)>(std::to_string),
 *         std::negate<int>())(7) << std::endl;
 *     // prints "-7!"
 *
 * Usage (II):
 *     std::vector<double> v(10), w(10),
 *     std::iota(v.begin(), v.end(), 1.0);
 *     std::iota(w.rbegin(), w.rend(), 1.0);
 *     std::transform(std::begin(v),
 *                    std::end(v),
 *                    std::begin(w),
 *                    std::ostream_iterator<double>(std::cout, ", "),
 *                    compose(
 *                        ::fabs,
 *                        ::sin,
 *                        ::atan2
 *                    )
 *                   );
 *     // prints: "1, 0.447214, 0.707107, 0.894427, 0.980581, "
 */
namespace detail
{

template<std::size_t N>
struct tuple_exec_ltr
{
    template<typename Arg, typename Tuple>
    static auto exec(Arg&& arg, Tuple&& tup)
    RETURNS_DECAY(
        std::get<N>(std::forward<Tuple>(tup))(tuple_exec_ltr<N-1>::exec(std::forward<Arg>(arg), std::forward<Tuple>(tup)))
    )
};

template<>
struct tuple_exec_ltr<0>
{
    template<typename Arg, typename Tuple>
    static auto exec(Arg&& arg, Tuple&& tup)
    RETURNS_DECAY(
        std::get<0>(std::forward<Tuple>(tup))(std::forward<Arg>(arg))
    )
};

template<std::size_t N>
struct tuple_exec_rtl
{
    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(Tuple&& tup, Arg&& arg, Args&&... args)
    RETURNS_DECAY(
        tuple_exec_rtl<N-1>::exec(std::forward<Tuple>(tup), std::get<N>(std::forward<Tuple>(tup))(std::forward<Arg>(arg), std::forward<Args>(args)...))
    )
};

template<>
struct tuple_exec_rtl<0>
{
    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(Tuple&& tup, Arg&& arg, Args&&... args)
    RETURNS_DECAY(
        std::get<0>(std::forward<Tuple>(tup))(std::forward<Arg>(arg), std::forward<Args>(args)...)
    )
};

} /* namespace detail */

template <typename... Fs>
struct composed
{
    std::tuple<Fs...> fs;

    explicit composed(Fs... fs) :
        fs(std::make_tuple(std::move(fs)...))
    {}

    template<typename Arg, typename... Args>
    auto operator()(Arg&& arg, Args&&... args)
    RETURNS_DECAY(
        detail::tuple_exec_rtl<sizeof...(Fs)-1>::exec(fs, std::forward<Arg>(arg), std::forward<Args>(args)...)
    )
};

template <typename F, typename... Fs>
composed<F, Fs...> compose(F f, Fs... fs)
{
    return composed<F, Fs...>(std::move(f), std::move(fs)...);
}

/*
 * Convenice generic ostream_iterator variant. In contrast to
 * std::ostream_iterator<T>, this class has a templated assigment operator
 * and a default first template parameter (void), which is ignored (as with
 * C++1y-style function objects from <functional>). Otherwise, usage is
 * identical to std::ostream_iterator<T>. The advantage is that the type
 * to be written to the specified std::ostream need not be specified, and
 * indeed the iterator may be reused with different types.
 *
 * Usage:
 *     auto os = ostreamer<>(std::cout, ", ");
 *     std::vector<int> v {0, 1, 1, 2, 3, 5, 8, 13};
 *     std::copy(v.begin(), v.end(), os);
 *     std::vector<const char*> v2 {"Hello", "World"};
 *     std::copy(v2.begin(), v2.end(), os);
 *     // prints: "0, 1, 1, 2, 3, 5, 8, 13, Hello, World, "
 */
template <class T = void, class CharT = char, class Traits = std::char_traits<CharT> >
class ostreamer
    : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
    typedef CharT char_type;
    typedef Traits traits_type;
    typedef std::basic_ostream<CharT,Traits> ostream_type;

    ostreamer(ostream_type& s)
        : os(&s), delim(0)
    {}

    ostreamer(ostream_type& s, const char_type* delimiter)
        : os(&s), delim(delimiter)
    {}

    template<typename Tp>
    ostreamer& operator=(const Tp& value)
    {
        *os << value;
        if (delim)
            *os << delim;
        return *this;
    }

    ostreamer& operator*()     {return *this;}
    ostreamer& operator++()    {return *this;}
    ostreamer& operator++(int) {return *this;}

private:
    ostream_type* os;
    const char_type* delim;
};

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
namespace math {

#define ALNR_UNARY_FUNCTOR(name) \
template <typename X = void> \
struct name { \
    template<typename T> \
    auto operator()(T&& t) const \
    RETURNS_AS_IS(std::name(std::forward<T>(t))) \
}

ALNR_UNARY_FUNCTOR(sin);
ALNR_UNARY_FUNCTOR(cos);
ALNR_UNARY_FUNCTOR(tan);
ALNR_UNARY_FUNCTOR(asin);
ALNR_UNARY_FUNCTOR(acos);
ALNR_UNARY_FUNCTOR(atan);
ALNR_UNARY_FUNCTOR(abs);
// more to come

#undef ALNR_UNARY_FUNCTOR

#define ALNR_BINARY_FUNCTOR(name) \
template <typename X = void> \
struct name { \
    template<typename T, typename U> \
    auto operator()(T&& t, U&& u) const \
    RETURNS_AS_IS(std::name(std::forward<T>(t), std::forward<U>(u))) \
}

ALNR_BINARY_FUNCTOR(atan2);
ALNR_BINARY_FUNCTOR(hypot);
// more to come

#undef ALNR_BINARY_FUNCTOR

} /* namespace math */

#undef RETURNS_AS_IS
#undef RETURNS_DECAY

#endif

