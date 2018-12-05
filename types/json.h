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

#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <variant>
#include <memory>
#include <cassert>
#include <type_traits>

#include "utils/type.h"
#include "utils/stream.h"

#ifndef LAK_JSON_WHITESPACE
#define LAK_JSON_WHITESPACE " \n\r\t"
#endif // LAK_JSON_WHITESPACE

#ifndef LAK_JSON_H
#define LAK_JSON_H

namespace lak
{
    using std::map;
    using std::vector;
    using std::string;
    using std::istream;
    using std::ostream;
    using std::stringstream;
    using std::variant;
    using std::holds_alternative;
    using std::get;
    using std::disjunction_v;
    using std::is_same;
    using std::is_same_v;
    using std::remove_reference_t;
    using std::enable_if_t;

    template<typename T, typename ...V>
    inline T &get_checked(variant<V...> &value)
    {
        // XXX - Implement runtime checking?
        assert(holds_alternative<T>(value));
        return get<T>(value);
    }

    template<typename T, typename ...V>
    inline const T &get_checked(const variant<V...> &value)
    {
        // XXX - Implement runtime checking?
        assert(holds_alternative<T>(value));
        return get<T>(value);
    }

    template<typename T> struct fake_t {};
    template<typename T, typename V> struct get_index;
    template<typename T, typename ...V> struct get_index<T, variant<V...>>
    { static constexpr size_t value = variant<fake_t<V>...>{fake_t<T>{}}.index(); };
    template<typename T, typename V>
    static constexpr size_t get_index_v = get_index<T, V>::value;

    template<template<typename, typename> typename OBJECT, template<typename> typename ARRAY, typename STRING, typename NUMBER, typename BOOLEAN, typename NULLT> struct json_t;

    template<template<typename, typename> typename OBJECT, template<typename> typename ARRAY, typename ...STRINGS, typename ...NUMBERS, typename BOOLEAN, typename NULLT>
    struct json_t<OBJECT, ARRAY, variant<STRINGS...>, variant<NUMBERS...>, BOOLEAN, NULLT>
    {
        using string_t = variant<STRINGS...>;       // std::variant<std::string, ...>
        using number_t = variant<NUMBERS...>;       // std::variant<double, float, int, ...>
        using boolean_t = BOOLEAN;                  // bool
        using null_t = NULLT;                       // nullptr_t
        using object_t = OBJECT<string, json_t>;    // std::map like
        using array_t = ARRAY<json_t>;              // std::vector like
        using value_t = variant<object_t, array_t, string_t, number_t, boolean_t, null_t>;

        value_t value;

        template<typename T>
        inline bool holds()
        {
            switch (value.index())
            {
                case get_index_v<number_t, value_t>:
                    if constexpr (disjunction_v<is_same<remove_reference_t<T>, NUMBERS>...>)
                        return holds_alternative<remove_reference_t<T>>(get<number_t>(value));
                    else return false;
                break;
                case get_index_v<string_t, value_t>:
                    if constexpr (disjunction_v<is_same<remove_reference_t<T>, STRINGS>...>)
                        return holds_alternative<remove_reference_t<T>>(get<string_t>(value));
                    else return false;
                break;
                default:
                    if constexpr (disjunction_v<
                        is_same<remove_reference_t<T>, object_t>,
                        is_same<remove_reference_t<T>, array_t>,
                        is_same<remove_reference_t<T>, string_t>,
                        is_same<remove_reference_t<T>, number_t>,
                        is_same<remove_reference_t<T>, boolean_t>,
                        is_same<remove_reference_t<T>, null_t>
                    >)
                        return holds_alternative<remove_reference_t<T>>(value);
                    else return false;
                break;
            }
        }

        json_t();
        ~json_t();

        inline json_t &operator[](const string &key)    { return get_checked<object_t>(value)[key]; }
        inline json_t &operator[](const size_t &index)  { return get_checked<array_t>(value)[index]; }
        inline const json_t &operator[](const string &key) const    { return get_checked<object_t>(value)[key]; }
        inline const json_t &operator[](const size_t &index) const  { return get_checked<array_t>(value)[index]; }

        inline json_t *operator()(const string &key)
        {
            object_t &obj = get_checked<object_t>(value);
            auto &&it = obj.find(key);
            return it != obj.end() ? &(it->second) : nullptr;
        }
        inline json_t *operator()(const size_t &index)
        {
            array_t &arr = get_checked<array_t>(value);
            return index < arr.size() ? &(arr[index]) : nullptr;
        }
        inline const json_t *operator()(const string &key) const
        {
            const object_t &obj = get_checked<object_t>(value);
            const auto &it = obj.find(key);
            return it != obj.end() ? &(it->second) : nullptr;
        }
        inline const json_t *operator()(const size_t &index) const
        {
            const array_t &arr = get_checked<array_t>(value);
            return index < arr.size() ? &(arr[index]) : nullptr;
        }

        template<typename T>
        json_t(const T &rhs)
        {
            if constexpr(is_same_v<remove_reference_t<T>, json_t>)
                value = rhs.value;
            else if constexpr(disjunction_v<is_same<remove_reference_t<T>, NUMBERS>...>)
                value = number_t{rhs};
            else if constexpr(disjunction_v<is_same<remove_reference_t<T>, STRINGS>...>)
                value = string_t{rhs};
            else if constexpr (disjunction_v<
                is_same<remove_reference_t<T>, object_t>,
                is_same<remove_reference_t<T>, array_t>,
                is_same<remove_reference_t<T>, string_t>,
                is_same<remove_reference_t<T>, number_t>,
                is_same<remove_reference_t<T>, boolean_t>,
                is_same<remove_reference_t<T>, null_t>
            >)
                value = rhs;
            else {
                static_assert(always_false_v<T>, "Bad type");
                // assert(false);
            }
        }

        template<typename T>
        json_t &operator=(const T &rhs)
        {
            if constexpr(is_same_v<remove_reference_t<T>, json_t>)
                value = rhs.value;
            else if constexpr(disjunction_v<is_same<remove_reference_t<T>, NUMBERS>...>)
                value = number_t{rhs};
            else if constexpr(disjunction_v<is_same<remove_reference_t<T>, STRINGS>...>)
                value = string_t{rhs};
            else if constexpr (disjunction_v<
                is_same<remove_reference_t<T>, object_t>,
                is_same<remove_reference_t<T>, array_t>,
                is_same<remove_reference_t<T>, string_t>,
                is_same<remove_reference_t<T>, number_t>,
                is_same<remove_reference_t<T>, boolean_t>,
                is_same<remove_reference_t<T>, null_t>
            >)
                value = rhs;
            else {
                static_assert(always_false_v<T>, "Bad type");
                // assert(false);
            }
            return *this;
        }

        template<typename T>
        explicit inline operator T&()
        {
            if constexpr (is_same_v<remove_reference_t<T>, json_t>)
                return *this;
            else if constexpr (is_same_v<remove_reference_t<T>, value_t>)
                return value;
            else if constexpr (is_same_v<remove_reference_t<T>, object_t>)
                return get_checked<object_t>(value);
            else if constexpr (is_same_v<remove_reference_t<T>, array_t>)
                return get_checked<array_t>(value);
            else if constexpr (is_same_v<remove_reference_t<T>, string_t>)
                return get_checked<string_t>(value);
            else if constexpr (is_same_v<remove_reference_t<T>, number_t>)
                return get_checked<number_t>(value);
            else if constexpr (is_same_v<remove_reference_t<T>, boolean_t>)
                return get_checked<boolean_t>(value);
            else if constexpr (is_same_v<remove_reference_t<T>, null_t>)
                return get_checked<null_t>(value);
            else if constexpr (disjunction_v<is_same<remove_reference_t<T>, NUMBERS>...>)
                return get_checked<remove_reference_t<T>>(get_checked<number_t>(value));
            else if constexpr (disjunction_v<is_same<remove_reference_t<T>, STRINGS>...>)
                return get_checked<remove_reference_t<T>>(get_checked<string_t>(value));
            else static_assert(always_false_v<T>, "Bad type");
        }

        template<typename T> explicit inline operator T() const { return (remove_reference_t<T>&)*this; }
    };

    template<template<typename, typename> typename OBJECT, template<typename> typename ARRAY, typename ...STRINGS, typename ...NUMBERS, typename BOOLEAN, typename NULLT>
    json_t<OBJECT, ARRAY, variant<STRINGS...>, variant<NUMBERS...>, BOOLEAN, NULLT>::json_t() : value(json_t::object_t{}) {}

    template<template<typename, typename> typename OBJECT, template<typename> typename ARRAY, typename ...STRINGS, typename ...NUMBERS, typename BOOLEAN, typename NULLT>
    json_t<OBJECT, ARRAY, variant<STRINGS...>, variant<NUMBERS...>, BOOLEAN, NULLT>::~json_t() {}

    using _number_t = variant<
        double, float, // long double,
        uint64_t, int64_t,
        uint32_t, int32_t,
        uint16_t, int16_t,
        uint8_t, int8_t
    >;

    using _string_t = variant<string>;

    using JSON = json_t<map, vector, _string_t, _number_t, bool, nullptr_t>;

    template<template<typename, typename> typename OBJ, template<typename> typename VEC, typename STR, typename NUM, typename BOOL, typename NUL>
    ostream& operator<<(ostream& os, const lak::json_t<OBJ, VEC, STR, NUM, BOOL, NUL> &json)
    {
        using namespace lak;
        using this_t = json_t<OBJ, VEC, STR, NUM, BOOL, NUL>;
        using object_t = typename this_t::object_t;
        using array_t = typename this_t::array_t;
        using string_t = typename this_t::string_t;
        using number_t = typename this_t::number_t;
        using boolean_t = typename this_t::boolean_t;
        using null_t = typename this_t::null_t;
        using value_t = typename this_t::value_t;

        switch(json.value.index())
        {
            case get_index_v<boolean_t, value_t>: {
                if ((boolean_t&)json)
                    os << "true";
                else
                    os << "false";
            } break;
            case get_index_v<string_t, value_t>: {
                // TODO: variadic switch
                string_t &strs = (string_t&)json;
                switch(strs.index())
                {
                    case get_index_v<string, string_t>: {
                        os << (string&)json;
                    }
                }
            } break;
            case get_index_v<number_t, value_t>: {
                // TODO: variadic switch
                number_t &nums = (number_t&)json;
                switch(nums.index())
                {
                    case get_index_v<double, number_t>: {
                        os << (double&)json;
                    } break;
                    case get_index_v<float, number_t>: {
                        os << (float&)json;
                    } break;
                    case get_index_v<uint64_t, number_t>: {
                        os << (uint64_t&)json;
                    } break;
                    case get_index_v<int64_t, number_t>: {
                        os << (int64_t&)json;
                    } break;
                    case get_index_v<uint32_t, number_t>: {
                        os << (uint32_t&)json;
                    } break;
                    case get_index_v<int32_t, number_t>: {
                        os << (int32_t&)json;
                    } break;
                    case get_index_v<uint16_t, number_t>: {
                        os << (uint16_t&)json;
                    } break;
                    case get_index_v<int16_t, number_t>: {
                        os << (int16_t&)json;
                    } break;
                    case get_index_v<uint8_t, number_t>: {
                        os << (uint8_t&)json;
                    } break;
                    case get_index_v<int8_t, number_t>: {
                        os << (int8_t&)json;
                    } break;
                }
            } break;
            case get_index_v<object_t, value_t>: {
                os << "{";
                object_t &obj = (object_t&)json;
                for (auto &it = obj.keys.begin(); it != obj.keys.end();)
                {
                    os << "\"" << it->first << "\" : " << obj.values[it->second];
                    if ((++it) != obj.keys.end())
                        os << ", ";
                }
                os << "}";
            } break;
            case get_index_v<array_t, value_t>: {
                os << "[";
                array_t &arr = (array_t&)json.value;
                for (auto &it = arr.begin(); it != arr.end();)
                {
                    os << *it;
                    if ((++it) != arr.end())
                        os << ", ";
                }
                os << "]";
            } break;
            case get_index_v<null_t, value_t>:
            default: {
                os << "null";
            } break;
        }
        return os;
    }
    template<template<typename, typename> typename OBJ, template<typename> typename VEC, typename STR, typename NUM, typename BOOL, typename NUL>
    istream& operator>>(istream& is, lak::json_t<OBJ, VEC, STR, NUM, BOOL, NUL> &json)
    {
        using namespace lak;
        using this_t = json_t<OBJ, VEC, STR, NUM, BOOL, NUL>;
        using object_t = typename this_t::object_t;
        using array_t = typename this_t::array_t;
        using string_t = typename this_t::string_t;
        using number_t = typename this_t::number_t;
        using boolean_t = typename this_t::boolean_t;
        using null_t = typename this_t::null_t;
        using value_t = typename this_t::value_t;

        skipAll(is, LAK_JSON_WHITESPACE);
        switch(is.peek())
        {
            // object
            case '{': {
                object_t &obj = (object_t&)(json = object_t{});
                obj.clear();
                skipNone(is, "\'\"}");
                istream::char_type c;
                while ((c = is.peek()) != '}')
                {
                    if (c == ',')
                    {
                        is.get();
                        skipAll(is, LAK_JSON_WHITESPACE);
                    }
                    string str = getString<char>(is);
                    skipOne(is, ":");
                    is >> obj[str];
                    skipAll(is, LAK_JSON_WHITESPACE);
                }
                is.get(); // remove '}'
            } break;
            // array
            case '[': {
                array_t &arr = (array_t&)(json = array_t{});
                arr.clear();
                istream::char_type c = is.get();
                while ((c = is.peek()) != ']')
                {
                    if (c == ',') is.get();
                    this_t v;
                    is >> v;
                    arr.push_back(v);
                    skipAll(is, LAK_JSON_WHITESPACE);
                }
                is.get(); // remove ']'
            } break;
            // string
            case '\'':
            case '\"': {
                json = string_t{getString<char>(is)};
            } break;
            // true
            case 'T':
            case 't': {
                json = boolean_t{true};
                skipAll(is, "TRUEtrue");
            } break;
            // false
            case 'F':
            case 'f': {
                json = boolean_t{false};
                skipAll(is, "FALSEfalse");
            } break;
            // null
            case 'N':
            case 'n':
            // undefined
            case 'U':
            case 'u': {
                json = null_t{};
                skipAll(is, "NULnulDEFIdefi");
            } break;
            // number
            default: {
                skipAll(is, LAK_JSON_WHITESPACE);
                json = number_t{getNumber<double>(is)};
            } break;
        }
        return is;
    }

    static JSON operator "" _JSON(const char* str, const size_t size)
    {
        const string &s = string{str};
        stringstream strm(str);
        JSON json{};
        strm >> json;
        return json;
    }
}
#endif // LAK_JSON_H