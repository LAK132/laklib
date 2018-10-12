/*
MIT License

Copyright (c) 2018 Lucas Kleiss (LAK132)

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

#include <cstddef>
#include <memory>
#include <type_traits>

#include "utils/type.h"

#ifndef LAK_TYPE_LIST_H
#define LAK_TYPE_LIST_H

namespace lak
{
    struct type_list_null { };
    template<typename ...ELEMS> struct type_list;
    template<typename LIST, typename ...ELEMS> struct reverse_type_list;
    template<typename ...ELEMS> struct type_list_struct;

    // LIST allows for use of types other than type_list
    // assuming they implement tail_t like type_list
    // eg: type_list_index<int, type_list_struct<string, int>>
    template<typename ELEM, typename LIST>
    struct type_list_index
    {
        enum { index = type_list_index<ELEM, typename LIST::tail_t>::index };
    };
    template<typename ELEM, template<typename, typename...> typename LIST, typename ...TAIL>
    struct type_list_index<ELEM, LIST<ELEM, TAIL...>>
    {
        enum { index = type_list<ELEM, TAIL...>::index };
    };
    // Enable if type list can be longer than size_t(-1)
    // or if you want compile time OOR checking
    #ifdef LAK_TYPE_LIST_OUT_OF_RANGE
    template<typename ELEM>
    struct type_list_index<ELEM, type_list_null>
    {
        enum { index = size_t(-1) };
    };
    #endif // LAK_TYPE_LIST_NO_OUT_OF_RANGE

    template<size_t INDEX, typename LIST>
    struct type_list_type_at
    {
        using result =
            typename std::conditional<
                INDEX == LIST::index,
                typename LIST::head_t,
                typename type_list_type_at<INDEX, typename LIST::tail_t>::result
            >::type;
    };
    template<size_t INDEX>
    struct type_list_type_at<INDEX, type_list_null>
    {
        using result = type_list_null;
    };
    template<size_t INDEX, typename LIST>
    using type_list_type_at_reverse = type_list_type_at<LIST::index - INDEX, LIST>;

    template<typename HEAD, typename ...TAIL>
    struct type_list<HEAD, TAIL...>
    {
        using head_t = HEAD;
        using tail_t = type_list<TAIL...>;
        template<size_t INDEX>
        using type_at = typename type_list_type_at<INDEX, type_list<HEAD, TAIL...>>::result;
        enum { index = tail_t::index + 1 };
    };
    template<typename HEAD>
    struct type_list<HEAD>
    {
        using head_t = HEAD;
        using tail_t = type_list_null;
        template<size_t INDEX>
        using type_at = typename type_list_type_at<INDEX, type_list<HEAD>>::result;
        enum { index = size_t(0) };
    };

    // Reverses the type list order
    template<typename HEAD, typename ...TAIL, typename ...ELEMS>
    struct reverse_type_list<type_list<HEAD, TAIL...>, ELEMS...>
    {
        using list_t = typename reverse_type_list<type_list<TAIL...>, HEAD, ELEMS...>::list_t;
    };
    template<typename HEAD, typename ...ELEMS>
    struct reverse_type_list<type_list<HEAD>, ELEMS...>
    {
        using list_t = type_list<HEAD, ELEMS...>;
    };

    // Creates a type list with elements in forward order (type_list<0, 1, 2...>)
    template<typename ...ELEMS>
    using forward_type_list = typename reverse_type_list<type_list<ELEMS...>>::list_t;
}

#endif // LAK_TYPE_LIST_H