/*
MIT License

Copyright (c) 2018 LAK132

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <utility>
#include <type_traits>

#ifndef LAK_TYPE_UTIL_H
#define LAK_TYPE_UTIL_H

namespace lak
{
    using std::declval;
    using std::decay;

    // Use when you otherwise need need constexpr static_assert(false)
    template<typename T> struct always_false { static constexpr bool value = false; };
    template<typename T> static constexpr bool always_false_v = always_false<T>::value;

    // Returns the first template type if a template, else return the base type
    template<typename T>
    struct _template_t
    {
        using type = T;
    };
    template<template<typename> typename T, typename U>
    struct _template_t<T<U>>
    {
        using type = U;
    };
    template<typename T>
    using template_t = typename _template_t<T>::type;

    template<typename T>
    struct _template_base_t
    {
        using type = T;
    };
    template<template<typename> typename T, typename U>
    struct _template_base_t<T<U>>
    {
        using type = typename _template_base_t<U>::type;
    };
    template<typename T>
    using template_base_t = typename _template_base_t<T>::type;

    template<typename L, typename R>
    using bigger_t = decltype(
        declval<typename decay<typename template_t<L>::type>::type>() *
        declval<typename decay<typename template_t<R>::type>::type>()
    );

    template<typename T, typename LIST>
    struct _has_type;
    template<typename T, template<typename...> typename LIST, typename ...Ts>
    struct _has_type<T, LIST<Ts...>> : public std::disjunction<std::is_same<T, Ts>...> {};
    template<typename T, typename LIST>
    static constexpr bool has_type = _has_type<T, LIST>::value;

    template<typename L, typename R, typename T = void>
    struct _has_rhs_mult
    {
        static constexpr bool value = false;
    };
    template<typename L, typename R>
    struct _has_rhs_mult<L, R, decltype(std::declval<L>().operator*(std::declval<R>()))>
    {
        static constexpr bool value = true;
    };
    template<typename L, typename R>
    static constexpr bool has_rhs_mult = _has_rhs_mult<L, R>::value;
}

#endif // LAK_TYPE_UTIL_H