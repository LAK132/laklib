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
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <variant>
#include <memory>
#include <cassert>

#include "utils/ldebug.h"
#include "utils/stream.h"
#include "utils/type.h"

#ifndef LAK_JSON_WHITESPACE
#define LAK_JSON_WHITESPACE " \n\r\t"
#endif // LAK_JSON_WHITESPACE

#ifndef LAK_JSON_H
#define LAK_JSON_H

namespace lak
{
    namespace JSON
    {
        using std::unordered_map;
        using std::vector;
        using std::string;
        using std::istream;
        using std::ostream;
        using std::stringstream;
        using std::shared_ptr;
        using std::variant;

        template<typename KEY, typename VALUE, typename SIZE = size_t>
        struct _object
        {
            unordered_map<KEY, SIZE> keys;
            vector<VALUE> values;
            VALUE& operator[](KEY key)
            {
                const auto &it = keys.find(key);
                if (it != keys.end())
                {
                    return values[it->second];
                }
                else
                {
                    values.resize(values.size()+1);
                    keys[key] = values.size()-1;
                    return values[keys[key]];
                }
            }
            const VALUE& operator[](KEY key) const
            {
                return values[keys.at(key)];
            }
            void clear() { keys.clear(); values.clear(); }
            void erase(KEY key)
            {
                const auto &it = keys.find(key);
                if (it != keys.end())
                {
                    values.erase(values.begin() + it->second);
                    keys.erase(it);
                }
            }
        };

        struct value_t
        {
            using string_t = string;
            using object_t = _object<string_t, value_t>;//unordered_map<string, value_t>;
            using array_t = vector<value_t>;
            // using number_t = double;
            using number_t = variant<
                double, float,
                uint64_t, int64_t,
                uint32_t, int32_t,
                uint16_t, int16_t,
                uint8_t, int8_t //, long double
            >;
            using pointer_t = shared_ptr<void>;
            struct null_t {};
            using type_t = variant<
                null_t,
                bool,
                string_t,
                number_t,
                object_t,
                array_t,
                pointer_t
            >;
            type_t val;

            enum type { JNULL = 0, BOOL = 1, STRING = 2, NUMBER = 3, OBJECT = 4, ARRAY = 5, POINTER = 6 };
            enum numtype { DOUBLE = 0, FLOAT = 1, UINT64 = 2, INT64 = 3, UINT32 = 4, INT32 = 5, UINT16 = 6, INT16 = 7, UINT8 = 8, INT8 = 9 };//, LDOUBLE = 10 };

            value_t();
            value_t(const value_t& other);
            value_t(value_t&& other);
            ~value_t();

            template<typename T>
            number_t& setNumType()
            {
                if (val.index() == NUMBER)
                {
                    switch(std::get<number_t>(val).index())
                    {
                        // case numtype::LDOUBLE: {
                        //     std::get<number_t>(val) = (T) std::get<numtype::LDOUBLE>(std::get<number_t>(val));
                        // } break;
                        case numtype::DOUBLE: {
                            std::get<number_t>(val) = (T) std::get<numtype::DOUBLE>(std::get<number_t>(val));
                        } break;
                        case numtype::FLOAT: {
                            std::get<number_t>(val) = (T) std::get<numtype::FLOAT>(std::get<number_t>(val));
                        } break;
                        case numtype::UINT64: {
                            std::get<number_t>(val) = (T) std::get<numtype::UINT64>(std::get<number_t>(val));
                        } break;
                        case numtype::INT64: {
                            std::get<number_t>(val) = (T) std::get<numtype::INT64>(std::get<number_t>(val));
                        } break;
                        case numtype::UINT32: {
                            std::get<number_t>(val) = (T) std::get<numtype::UINT32>(std::get<number_t>(val));
                        } break;
                        case numtype::INT32: {
                            std::get<number_t>(val) = (T) std::get<numtype::INT32>(std::get<number_t>(val));
                        } break;
                        case numtype::UINT16: {
                            std::get<number_t>(val) = (T) std::get<numtype::UINT16>(std::get<number_t>(val));
                        } break;
                        case numtype::INT16: {
                            std::get<number_t>(val) = (T) std::get<numtype::INT16>(std::get<number_t>(val));
                        } break;
                        case numtype::UINT8: {
                            std::get<number_t>(val) = (T) std::get<numtype::UINT8>(std::get<number_t>(val));
                        } break;
                        case numtype::INT8: {
                            std::get<number_t>(val) = (T) std::get<numtype::INT8>(std::get<number_t>(val));
                        } break;
                    }
                }
                else
                {
                    val = number_t();
                    get<number_t>() = T();
                }
                return get<number_t>();
            }
            template<size_t T>
            auto& setType()
            {
                switch(T)
                {
                    case JNULL: val = null_t(); break;
                    case BOOL: val = bool(false); break;
                    case STRING: val = string_t(""); break;
                    // case NUMBER: val = number_t(0); break;
                    case NUMBER: val = number_t(); break;
                    // case OBJECT: val = object_t(0); break;
                    case OBJECT: val = object_t(); break;
                    case ARRAY: val = array_t(0); break;
                    case POINTER: val = pointer_t(); break;
                }
                return std::get<T>(val);
            }

            template<typename T>
            shared_ptr<T>& operator=(shared_ptr<T>& other)
            {
                val = (pointer_t)other;
                return std::get<shared_ptr<T>>(val);
            }
            template<typename T>
            shared_ptr<T>& operator=(shared_ptr<T>&& other)
            {
                val = (pointer_t)other;
                return std::get<shared_ptr<T>>(val);
            }

            template<typename T>
            inline typename std::enable_if<has_type<T, number_t>, bool>::type is_type() const
            {
                if (val.index() == NUMBER)
                    return std::holds_alternative<T>(std::get<number_t>(val));
                else
                    return false;
            }
            template<typename T>
            inline typename std::enable_if<has_type<T, type_t>, bool>::type is_type() const
            {
                return std::holds_alternative<T>(val);
            }
            template<typename T>
            inline typename std::enable_if<!has_type<T, number_t> && !has_type<T, type_t>, bool>::type is_type(...) const
            {
                return false;
            }

            template<typename T>
            inline typename std::enable_if<has_type<T, number_t>, T>::type& get()
            {
                if (val.index() != NUMBER)
                    val = number_t();
                if (!std::holds_alternative<T>(std::get<number_t>(val)))
                    setNumType<T>();
                return std::get<T>(std::get<number_t>(val));
            }
            template<typename T>
            inline typename std::enable_if<has_type<T, type_t>, T>::type& get()
            {
                if (!is_type<T>()) val = T();
                return std::get<T>(val);
            }
            template<typename T>
            inline typename std::enable_if<!has_type<T, number_t> && !has_type<T, type_t>, T>::type& get()
            {
                T t;
                *this >> t;
                return t;
            }
            template<size_t T>
            inline auto& get()
            {
                // if (!is_type<T>()) setType<T>();
                return std::get<T>(val);
            }
            template<size_t T>
            inline auto& getnum()
            {
                // if (!is_type<T>()) setType<T>();
                return std::get<T>(std::get<number_t>(val));
            }

            // template<typename T>
            // inline const typename std::enable_if<has_type<T, number_t>, T>::type& cget() const
            // {
            //     return std::get<T>(std::get<number_t>(val));
            // }
            // template<typename T>
            // inline const typename std::enable_if<has_type<T, type_t>, T>::type& cget() const
            // {
            //     return std::get<T>(val);
            // }
            template<typename T>
            inline const typename std::enable_if<has_type<T, number_t>, T>::type& cget() const
            {
                // if (!is_type<T>()) {val = number_t(); setNumType<T>();}
                assert(is_type<T>());
                return std::get<T>(std::get<number_t>(val));
            }
            template<typename T>
            inline const typename std::enable_if<has_type<T, type_t>, T>::type& cget() const
            {
                // if (!is_type<T>()) val = T();
                assert(is_type<T>());
                return std::get<T>(val);
            }
            template<typename T>
            inline const typename std::enable_if<!has_type<T, number_t> && !has_type<T, type_t>, T>::type& cget() const
            {
                T t;
                *this >> t;
                return t;
            }
            template<size_t T>
            inline const auto& cget() const
            {
                return std::get<T>(val);
            }
            template<size_t T>
            inline const auto& cgetnum() const
            {
                return std::get<T>(std::get<number_t>(val));
            }

            value_t& operator[](const size_t index);
            value_t& operator[](const string_t index);
            pointer_t& operator*();
            const value_t& operator[](const size_t index) const;
            const value_t& operator[](const string_t index) const;
            const pointer_t& operator*() const;

            value_t& operator=(const value_t& other);
            value_t& operator=(value_t&& other);
            null_t& operator=(const null_t& other);
            null_t& operator=(null_t&& other);
            bool& operator=(const bool& other);
            bool& operator=(bool&& other);
            string_t& operator=(const string_t& other);
            string_t& operator=(string_t&& other);
            string_t& operator=(const char* other);
            number_t& operator=(const number_t& other);
            number_t& operator=(number_t&& other);
            object_t& operator=(const object_t& other);
            object_t& operator=(object_t&& other);
            array_t& operator=(const array_t& other);
            array_t& operator=(array_t&& other);

            friend ostream& operator<<(ostream& os, const value_t& val);
            friend istream& operator>>(istream& is, value_t& val);
        };

        using JSON = value_t;

        static JSON operator "" _JSON(const char* str, const size_t size)
        {
            const string s = str;
            stringstream strm(str);
            JSON json;
            strm >> json;
            return json;
        }
    }
}

// #define LAK_JSON_IMPLEM
#ifdef LAK_JSON_IMPLEM
#   ifndef LAK_JSON_HAS_IMPLEM
#       define LAK_JSON_HAS_IMPLEM
#       include "types/json.cpp"
#   endif // LAK_JSON_HAS_IMPLEM
#endif // LAK_JSON_IMPLEM

#endif // LAK_JSON_H