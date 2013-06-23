# cpp

A collection of useful und generic functions and classes in C++11.
To use, specify -std=c++0x during your compiler invocation. Requires
support for variadic templates, late-specified return types, decltype,
and rvalue references.

Tested on compilers:
* G++ 4.8.1
* G++ 4.7.2
* Clang++ 3.3
* Clang++ 3.2
* Intel C++ 13.1.1

G++ 4.6 and earlier are unsupported.

### Function objects

Simple, yet useful function objects for programming with higher-order
functions, adapted from SGI's original STL. The design rationale follows
the recently adopted proposal [N3421](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3421.htm)
by Stephan T. Lavavej. The classes are templated with a default template 
parameter (`void`), which is ignored. Instead, `operator()` is templated to
provide (compile-time) polymorphism. All functions use perfect forwarding.

* `alnr::identity<>` The identity function. Takes one argument, and 
returns it unmodified.
* `alnr::project1st<>` Takes two arguments, and returns the first unmodified.
* `alnr::project2nd<>` Takes two arguments, and returns the second unmodified.
* `alnr::select1st<>` Takes one argument of tuple or pair type (anything that 
supports `std::get<N>`, really), and returns the first element.
* `alnr::select2nd<>` Takes one argument of tuple or pair type, and returns the 
second element.

### Function composition
* `alnr::compose_r()` Creates a function object of the right-leaning composition
of its arguments. `alnr::compose_r(f, g, h)(x)` thus evaluates to `f(g(h(x)))`.
* `alnr::compose_l()` Creates the left-leaning composition of its arguments, i.e.
`alnr::compose_l(f, g, h)(x)` evaluates to `h(g(f(x)))`.
All function arguments to `alnr::compose_r()` and `alnr::compose_l()` must take
exactly one argument and return one value, except for the fist function to evaluate
(the rightmost for the right-leaning composition and the leftmost for the left-leaning
composition), which may take more than one argument. In any case, the composition
takes as many arguments as the first function of the composition.
