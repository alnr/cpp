/*
 *    Copyright (c) 2013 Arne Luenser
 */

#ifndef F487DC3045F53649BE3584DDC8D06608771D375A9D2F7DDDE877FC02D20CCA92
#define F487DC3045F53649BE3584DDC8D06608771D375A9D2F7DDDE877FC02D20CCA92

#include <cstddef>
#include <tuple>
#include <utility>

namespace alnr 
{

#define ALNR_RETURNS(...) -> decltype(__VA_ARGS__) { return (__VA_ARGS__); }

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
    ALNR_RETURNS(
        std::get<0>(std::forward<P>(p))
    )
};

template <typename T = void>
struct select2nd
{
    template<typename P>
    auto operator()(P&& p) const
    ALNR_RETURNS(
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
    ALNR_RETURNS(
        std::forward<T1>(t1)
    )
};

template <typename T = void>
struct project1st
{
    template<typename T1, typename T2>
    auto operator()(T1&& t1, T2&&) const
    ALNR_RETURNS(
        std::forward<T1>(t1)
    )
};

template <typename T = void>
struct project2nd
{
    template<typename T1, typename T2>
    auto operator()(T1&&, T2&& t2) const
    ALNR_RETURNS(
        std::forward<T2>(t2)
    )
};

/*
 * Higher-order function: composition
 *
 * compose_r(f, g, h, ...) returns the right-leaning composition of the 
 * functions f, g, h, ... compose_r(f,g,h)(x) thus evaluates to f(g(h(x))).
 *
 * compose_l(f, g, h, ...) returns the corresponding left-leaning composition.
 * compose_l(f, g, h, ...)(x) evaluates to h(g(f(x))).
 * 
 * The first function to evaluate (i.e., the rightmost for compose_r() and the
 * leftmost for compose_l()) may take and arbitrary number of arguments, and
 * return one value. All other functions must take exactly one argument, and
 * return one value. Arbitrary argument and return types are allowed, provided
 * the composition as a whole has consistent typing (see example). The function
 * arguments may be any callable type, such as normal functions, function
 * objects, and lambdas.
 *
 * Note: overloaded functions require explicit casting to disambiguate
 * (see first example).
 *
 * Usage (I):
 *     std::cout << compose_r(
 *         [](std::string s) { return s + "!"; },
 *         static_cast<std::string(*)(int)>(std::to_string),
 *         std::negate<int>())(7) << std::endl;
 *     // prints "-7!"
 *
 * Usage (II):
 *     std::vector<double> v(5), w(5);
 *     std::iota(v.begin(), v.end(), 1.0);
 *     std::iota(w.rbegin(), w.rend(), 1.0);
 *     std::transform(std::begin(v),
 *                    std::end(v),
 *                    std::begin(w),
 *                    std::ostream_iterator<double>(std::cout, ", "),
 *                    compose_r(
 *                        ::fabs,
 *                        ::sin,
 *                        ::atan2 // takes two arguments
 *                    )
 *                   );
 *     // prints: "0.196116, 0.447214, 0.707107, 0.894427, 0.980581, "
 */
namespace detail
{

template<std::size_t N>
struct tuple_exec_ltr
{
    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(const Tuple& tup, Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        std::get<N>(tup)(tuple_exec_ltr<N-1>::exec(tup,
            std::forward<Arg>(arg), std::forward<Args>(args)...))
    )

    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(Tuple& tup, Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        std::get<N>(tup)(tuple_exec_ltr<N-1>::exec(tup,
            std::forward<Arg>(arg), std::forward<Args>(args)...))
    )
};

template<>
struct tuple_exec_ltr<0>
{
    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(const Tuple& tup, Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        std::get<0>(tup)(std::forward<Arg>(arg), std::forward<Args>(args)...)
    )

    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(Tuple& tup, Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        std::get<0>(tup)(std::forward<Arg>(arg), std::forward<Args>(args)...)
    )
};

template<std::size_t N>
struct tuple_exec_rtl
{
    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(const Tuple& tup, Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        tuple_exec_rtl<N-1>::exec(tup,
        std::get<N>(tup)(std::forward<Arg>(arg), std::forward<Args>(args)...))
    )

    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(Tuple& tup, Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        tuple_exec_rtl<N-1>::exec(tup,
        std::get<N>(tup)(std::forward<Arg>(arg), std::forward<Args>(args)...))
    )
};

template<>
struct tuple_exec_rtl<0>
{
    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(const Tuple& tup, Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        std::get<0>(tup)(std::forward<Arg>(arg), std::forward<Args>(args)...)
    )

    template<typename Tuple, typename Arg, typename... Args>
    static auto exec(Tuple& tup, Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        std::get<0>(tup)(std::forward<Arg>(arg), std::forward<Args>(args)...)
    )
};

} /* namespace detail */

template <typename... Fs>
struct composed_ltr
{
    std::tuple<Fs...> fs;

    explicit composed_ltr(Fs... fs) :
        fs(std::make_tuple(std::move(fs)...))
    {}

    template<typename Arg, typename... Args>
    auto operator()(Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        detail::tuple_exec_ltr<sizeof...(Fs)-1>::exec(fs, std::forward<Arg>(arg),
        std::forward<Args>(args)...)
    )

    template<typename Arg, typename... Args>
    auto operator()(Arg&& arg, Args&&... args) const
    ALNR_RETURNS(
        detail::tuple_exec_ltr<sizeof...(Fs)-1>::exec(fs, std::forward<Arg>(arg),
        std::forward<Args>(args)...)
    )
};

template <typename... Fs>
struct composed_rtl
{
    std::tuple<Fs...> fs;

    explicit composed_rtl(Fs... fs) :
        fs(std::make_tuple(std::move(fs)...))
    {}

    template<typename Arg, typename... Args>
    auto operator()(Arg&& arg, Args&&... args)
    ALNR_RETURNS(
        detail::tuple_exec_rtl<sizeof...(Fs)-1>::exec(fs, std::forward<Arg>(arg),
        std::forward<Args>(args)...)
    )

    template<typename Arg, typename... Args>
    auto operator()(Arg&& arg, Args&&... args) const
    ALNR_RETURNS(
        detail::tuple_exec_rtl<sizeof...(Fs)-1>::exec(fs, std::forward<Arg>(arg),
        std::forward<Args>(args)...)
    )
};

template <typename F, typename... Fs>
composed_ltr<F, Fs...> compose_l(F f, Fs... fs)
{
    return composed_ltr<F, Fs...>(std::move(f), std::move(fs)...);
}

template <typename F, typename... Fs>
composed_rtl<F, Fs...> compose_r(F f, Fs... fs)
{
    return composed_rtl<F, Fs...>(std::move(f), std::move(fs)...);
}

#undef ALNR_RETURNS

} /* namespace alnr */

#endif

