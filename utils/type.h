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
#include <vector>

#ifndef LAK_TYPE_UTIL_H
#define LAK_TYPE_UTIL_H

namespace lak
{
    // Use when you otherwise need need constexpr static_assert(false)
    template<typename T> struct always_false { static constexpr bool value = false; };
    template<typename T> static constexpr bool always_false_v = always_false<T>::value;
    struct bad_type;

    // Returns the first template type if a template, else return the base type
    template<typename T>
    struct templated_by
    { using type = T; };
    template<template<typename> typename T, typename U>
    struct templated_by<T<U>>
    { using type = U; };
    template<typename T>
    using templated_by_t = typename templated_by<T>::type;

    template<typename T>
    struct templated_by_base
    { using type = T; };
    template<template<typename> typename T, typename U>
    struct templated_by_base<T<U>>
    { using type = typename templated_by_base<U>::type; };
    template<typename T>
    using template_by_base_t = typename templated_by_base<T>::type;

    template<typename L, typename R>
    using bigger_t = decltype(
        std::declval<typename std::decay<typename templated_by_t<L>::type>::type>() *
        std::declval<typename std::decay<typename templated_by_t<R>::type>::type>()
    );

    template<typename T, typename LIST>
    struct has_type;
    template<typename T, template<typename...> typename LIST, typename ...Ts>
    struct has_type<T, LIST<Ts...>> : public std::disjunction<std::is_same<T, Ts>...> {};
    template<typename T, typename LIST>
    static constexpr bool has_type_v = has_type<T, LIST>::value;

    template<typename L, typename R, typename T = bad_type>
    struct has_rhs_mult
    { static constexpr bool value = false; };
    template<typename L, typename R>
    struct has_rhs_mult<L, R, decltype(std::declval<L>().operator*(std::declval<R>()))>
    { static constexpr bool value = true; };
    template<typename L, typename R>
    static constexpr bool has_rhs_mult_v = has_rhs_mult<L, R>::value;

    template<template<typename...> typename T, typename U, typename V = bad_type>
    struct is_template
    { static constexpr bool value = false; };
    template<template<typename...> typename T, typename ...U>
    struct is_template<T, T<U...>>
    { static constexpr bool value = true; };
    template<template<typename...> typename T, typename U>
    static constexpr bool is_template_v = is_template<T, U>::value;

    template<typename T>
    constexpr auto _complete_type(int) -> decltype(sizeof(T), bool{})
    { return true; }
    template<typename T>
    constexpr bool _complete_type(double)
    { return false; }
    template<typename T>
    constexpr bool complete_type()
    { return _complete_type<T>(0); }

    template<typename T = bad_type>
    struct valid_type
    { static constexpr bool value = complete_type<T>() && !std::is_same_v<T, bad_type>; };
    template<typename T = bad_type>
    static constexpr bool valid_type_v = valid_type<T>::value;
}

#endif // LAK_TYPE_UTIL_H