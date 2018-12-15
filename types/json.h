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
    using std::unordered_map;
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

    template<typename KEY, typename STRING, typename NUMBER, typename BOOLEAN, typename NULLT> struct json_t;

    template<typename KEY, typename ...STRINGS, typename ...NUMBERS, typename BOOLEAN, typename NULLT>
    struct json_t<KEY, variant<STRINGS...>, variant<NUMBERS...>, BOOLEAN, NULLT>
    {
        using object_t = map<KEY, json_t>;
        using array_t = vector<json_t>;
        using objkey_t = typename object_t::key_type;
        using arrkey_t = typename array_t::size_type;
        using string_t = variant<STRINGS...>;   // std::variant<std::string, ...>
        using number_t = variant<NUMBERS...>;   // std::variant<double, float, int, ...>
        using boolean_t = BOOLEAN;              // bool
        using null_t = NULLT;                   // nullptr_t
        using value_t = variant<object_t, array_t, string_t, number_t, boolean_t, null_t>;

        value_t value;

        template<typename T>
        inline bool holds() const
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

        template<typename T>
        static constexpr bool might_hold =
            disjunction_v<is_same<remove_reference_t<T>, NUMBERS>...> ||
            disjunction_v<is_same<remove_reference_t<T>, STRINGS>...> ||
            disjunction_v<
                is_same<remove_reference_t<T>, object_t>,
                is_same<remove_reference_t<T>, array_t>,
                is_same<remove_reference_t<T>, string_t>,
                is_same<remove_reference_t<T>, number_t>,
                is_same<remove_reference_t<T>, boolean_t>,
                is_same<remove_reference_t<T>, null_t>
            >;

        template<typename T>
        inline bool holds_close() const
        {
            switch (value.index())
            {
                case get_index_v<number_t, value_t>:
                    return disjunction_v<is_same<remove_reference_t<T>, NUMBERS>...>;
                break;
                case get_index_v<string_t, value_t>:
                    return disjunction_v<is_same<remove_reference_t<T>, STRINGS>...>;
                break;
                default:
                    return disjunction_v<
                        is_same<remove_reference_t<T>, object_t>,
                        is_same<remove_reference_t<T>, array_t>,
                        is_same<remove_reference_t<T>, string_t>,
                        is_same<remove_reference_t<T>, number_t>,
                        is_same<remove_reference_t<T>, boolean_t>,
                        is_same<remove_reference_t<T>, null_t>>;
                break;
            }
        }

        json_t();
        ~json_t();

        // subscript notation for object (string index) and array (number index) types
        inline json_t &operator[](const objkey_t &key)
        {
            return get_checked<object_t>(value)[key];
        }
        inline json_t &operator[](const arrkey_t &key)
        {
            return get_checked<array_t>(value)[key];
        }
        inline const json_t &operator[](const objkey_t &key) const
        {
            return get_checked<object_t>(value)[key];
        }
        inline const json_t &operator[](const arrkey_t &key) const
        {
            return get_checked<array_t>(value)[key];
        }

        // subscript notation for object and array types, but returns nullptr if index doesn't exist
        inline json_t *operator()(const objkey_t &key)
        {
            if (!holds<object_t>())
                return nullptr;
            object_t &obj = get_checked<object_t>(value);
            auto &&it = obj.find(key);
            return it != obj.end() ? &(it->second) : nullptr;
        }
        inline json_t *operator()(const arrkey_t &key)
        {
            if (!holds<array_t>())
                return nullptr;
            array_t &arr = get_checked<array_t>(value);
            return key < arr.size() ? &(arr[key]) : nullptr;
        }
        inline const json_t *operator()(const objkey_t &key) const
        {
            if (!holds<object_t>())
                return nullptr;
            const object_t &obj = get_checked<object_t>(value);
            const auto &it = obj.find(key);
            return it != obj.end() ? &(it->second) : nullptr;
        }
        inline const json_t *operator()(const arrkey_t &key) const
        {
            if (!holds<array_t>())
                return nullptr;
            const array_t &arr = get_checked<array_t>(value);
            return key < arr.size() ? &(arr[key]) : nullptr;
        }

        // type-safe copy from index into val, val not touched if incorrect type or non-existent index
        template<typename K, typename T>
        inline enable_if_t<is_same_v<K, objkey_t> || is_same_v<K, arrkey_t>,
        bool> operator()(const K &key, T &val) const
        {
            bool rtn = false;
            if (const json_t *obj = (*this)(key); obj)
            {
                // we use `if constexpr` here to avoid compiler errors from
                // bad syntax and/or function calls by checking if T is one
                // of the *possible* types for obj. the inner `obj->holds` then
                // checks the actual type at runtime
                if constexpr (is_template_v<vector, T>)
                {
                    val.clear(); // always clear
                    if (obj->holds<array_t>())
                    {
                        rtn = true; // only return true if key exists and type is correct
                        // val is an array type
                        const array_t &array = (const array_t&)*obj;
                        for (const json_t &a : array)
                        {
                            if (a.holds<typename T::value_type>())
                            {
                                if constexpr (might_hold<typename T::value_type>)
                                {
                                    val.push_back((const typename T::value_type&)a);
                                }
                                else
                                {
                                    val.emplace_back();
                                    val.back() = a;
                                }
                            }
                        }
                    }
                }
                else if constexpr (is_template_v<map, T> || is_template_v<unordered_map, T>)
                {
                    val.clear(); // always clear
                    if (obj->holds<object_t>())
                    {
                        rtn = true; // only return true if key exists and type is correct
                        // val is an object type
                        const object_t &object = (const object_t&)*obj;
                        for (const auto &[k, o] : object)
                        {
                            if (((const json_t&)o).holds<typename T::mapped_type>())
                            {
                                if constexpr (might_hold<typename T::mapped_type>)
                                {
                                    val[k] = (const typename T::mapped_type&)o;
                                }
                                else
                                {
                                    val[k] = o;
                                }
                            }
                        }
                    }
                }
                else if constexpr (might_hold<T>)
                {
                    if (obj->holds<T>())
                        val = (const T&)*obj; rtn = true;
                }
                else
                {
                    // T might have a `bool operator=(json)` function
                    // if not, this will probably throw a compiler error
                    rtn = (val = *obj); // if you can see this your type is missing an (type=json_t)->bool operator
                }
            }
            return rtn;
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
            else
                *this << rhs; // if you can see this your type is missing an json_t << type operator
        }

        template<typename T>
        inline json_t &operator=(const T &rhs)
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
            else
                *this << rhs; // if you can see this your type is missing an json_t << type operator
            return *this;
        }

        template<typename T>
        inline T& as()
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

        template<typename T>
        inline const T& as_const () const
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

        template<typename T> explicit inline operator T&() { return as<T>(); }
        template<typename T> explicit inline operator const T&() const { return as_const<T>(); }
        template<typename T> explicit inline operator T() const { return as_const<T>(); }
    };

    template<typename KEY, typename ...STRINGS, typename ...NUMBERS, typename BOOLEAN, typename NULLT>
    json_t<KEY, variant<STRINGS...>, variant<NUMBERS...>, BOOLEAN, NULLT>::json_t() : value(json_t::object_t{}) {}

    template<typename KEY, typename ...STRINGS, typename ...NUMBERS, typename BOOLEAN, typename NULLT>
    json_t<KEY, variant<STRINGS...>, variant<NUMBERS...>, BOOLEAN, NULLT>::~json_t() {}

    template<typename ...T> using json_object_t     = typename json_t<T...>::object_t;
    template<typename ...T> using json_objkey_t     = typename json_t<T...>::objkey_t;
    template<typename ...T> using json_array_t      = typename json_t<T...>::array_t;
    template<typename ...T> using json_arrkey_t     = typename json_t<T...>::arrkey_t;
    template<typename ...T> using json_objkey_t     = typename json_t<T...>::objkey_t;
    template<typename ...T> using json_string_t     = typename json_t<T...>::string_t;
    template<typename ...T> using json_number_t     = typename json_t<T...>::number_t;
    template<typename ...T> using json_boolean_t    = typename json_t<T...>::boolean_t;
    template<typename ...T> using json_null_t       = typename json_t<T...>::null_t;
    template<typename ...T> using json_value_t      = typename json_t<T...>::value_t;

    template<typename ...T>
    ostream& operator<<(ostream& os, const json_t<T...> &json)
    {
        using this_t    = json_t<T...>;
        using object_t  = json_object_t<T...>;
        using array_t   = json_array_t<T...>;
        using string_t  = json_string_t<T...>;
        using number_t  = json_number_t<T...>;
        using boolean_t = json_boolean_t<T...>;
        using null_t    = json_null_t<T...>;
        using value_t   = json_value_t<T...>;

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

    template<typename ...T>
    istream& operator>>(istream& is, json_t<T...> &json)
    {
        using this_t    = json_t<T...>;
        using object_t  = json_object_t<T...>;
        using array_t   = json_array_t<T...>;
        using string_t  = json_string_t<T...>;
        using number_t  = json_number_t<T...>;
        using boolean_t = json_boolean_t<T...>;
        using null_t    = json_null_t<T...>;
        using value_t   = json_value_t<T...>;

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

    // JSON -> basic implementation of json_t

    using _objkey_t = string;
    using _string_t = variant<_objkey_t>;
    using _number_t = variant<
        double, float, // long double,
        uint64_t, int64_t,
        uint32_t, int32_t,
        uint16_t, int16_t,
        uint8_t, int8_t
    >;
    using JSON = json_t<_objkey_t, _string_t, _number_t, bool, nullptr_t>;

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