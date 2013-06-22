/*
 *    Copyright (c) 2013 Arne Luenser
 */

#ifndef FC440D788BD7070003E5F5EFA53BB2E9C21BAB9E0F3E9F4A93D41D314E8212AF
#define FC440D788BD7070003E5F5EFA53BB2E9C21BAB9E0F3E9F4A93D41D314E8212AF

#include <iterator>
#include <ostream>

namespace alnr
{

/*
 * Convenience generic ostream_iterator variant. In contrast to
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
template <class T = void, class CharT = char,
          class Traits = std::char_traits<CharT> >
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

} /* namespace alnr */

#endif

