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

#include "types/json.h"

namespace lak
{
    namespace JSON
    {
        value_t::value_t()
        {
        }

        value_t::value_t(const value_t& other)
        {
            *this = other;
        }

        value_t::value_t(value_t&& other)
        {
            *this = other;
        }

        value_t::~value_t()
        {
            setType<JNULL>();
        }

        value_t& value_t::operator[](const size_t index)
        {
            if (val.index() != ARRAY)
                setType<ARRAY>().resize(index);
            return get<array_t>()[index];
        }

        value_t& value_t::operator[](const string_t index)
        {
            if (val.index() != OBJECT)
                setType<OBJECT>();
            return get<object_t>()[index];
        }

        value_t::pointer_t& value_t::operator*()
        {
            if (val.index() != POINTER)
                setType<POINTER>();
            return get<pointer_t>();
        }

        const value_t& value_t::operator[](const size_t index) const
        {
            assert(val.index() == ARRAY);
            return cget<array_t>()[index];
        }

        const value_t& value_t::operator[](const string_t index) const
        {
            assert(val.index() == OBJECT);
            return cget<object_t>()[index];
        }

        const value_t::pointer_t& value_t::operator*() const
        {
            assert(val.index() == POINTER);
            return cget<pointer_t>();
        }

        value_t& value_t::operator=(const value_t& other)
        {
            val = other.val;
            return *this;
        }

        value_t& value_t::operator=(value_t&& other)
        {
            val = other.val;
            return *this;
        }

        value_t::null_t& value_t::operator=(const null_t& other)
        {
            val = other;
            return std::get<null_t>(val);
        }
        value_t::null_t& value_t::operator=(null_t&& other)
        {
            val = other;
            return std::get<null_t>(val);
        }

        bool& value_t::operator=(const bool& other)
        {
            val = other;
            return std::get<bool>(val);
        }
        bool& value_t::operator=(bool&& other)
        {
            val = other;
            return std::get<bool>(val);
        }

        value_t::string_t& value_t::operator=(const string_t& other)
        {
            val = other;
            return std::get<string_t>(val);
        }
        value_t::string_t& value_t::operator=(string_t&& other)
        {
            val = other;
            return std::get<string_t>(val);
        }
        value_t::string_t& value_t::operator=(const char* other)
        {
            setType<STRING>() = other;
            return std::get<string_t>(val);//str;
        }

        value_t::number_t& value_t::operator=(const number_t& other)
        {
            val = other;
            return std::get<number_t>(val);
        }
        value_t::number_t& value_t::operator=(number_t&& other)
        {
            val = other;
            return std::get<number_t>(val);
        }

        value_t::object_t& value_t::operator=(const object_t& other)
        {
            val = other;
            return std::get<object_t>(val);
        }
        value_t::object_t& value_t::operator=(object_t&& other)
        {
            val = other;
            return std::get<object_t>(val);
        }

        value_t::array_t& value_t::operator=(const array_t& other)
        {
            val = other;
            return std::get<array_t>(val);
        }
        value_t::array_t& value_t::operator=(array_t&& other)
        {
            val = other;
            return std::get<array_t>(val);
        }

        ostream& operator<<(ostream& os, const value_t& val)
        {
            switch(val.val.index())
            {
                case value_t::JNULL: {
                    os << "null";
                } break;
                case value_t::BOOL: {
                    if (val.cget<bool>()) os << "true";
                    else os << "false";
                } break;
                case value_t::STRING: {
                    string str = val.cget<JSON::string_t>();
                    while(size_t i = str.find_first_of('\b') != string::npos) str.replace(i, 1, "\\b");
                    while(size_t i = str.find_first_of('\f') != string::npos) str.replace(i, 1, "\\f");
                    while(size_t i = str.find_first_of('\n') != string::npos) str.replace(i, 1, "\\n");
                    while(size_t i = str.find_first_of('\r') != string::npos) str.replace(i, 1, "\\r");
                    while(size_t i = str.find_first_of('\t') != string::npos) str.replace(i, 1, "\\t");
                    // use c_str() to ensure we don't accidentally read 0s at the end of the string
                    os << "\"" << val.cget<JSON::string_t>().c_str() << "\"";
                } break;
                case value_t::NUMBER: {
                    switch(val.cget<JSON::number_t>().index())
                    {
                        // case JSON::numtype::LDOUBLE: {
                        //     os << val.cgetnum<JSON::numtype::LDOUBLE>();
                        // } break;
                        case JSON::numtype::DOUBLE: {
                            os << val.cget<double>();
                        } break;
                        case JSON::numtype::FLOAT: {
                            os << val.cget<float>();
                        } break;
                        case JSON::numtype::UINT64: {
                            os << val.cget<uint64_t>();
                        } break;
                        case JSON::numtype::INT64: {
                            os << val.cget<int64_t>();
                        } break;
                        case JSON::numtype::UINT32: {
                            os << val.cget<int32_t>();
                        } break;
                        case JSON::numtype::INT32: {
                            os << val.cget<int32_t>();
                        } break;
                        case JSON::numtype::UINT16: {
                            os << val.cget<uint16_t>();
                        } break;
                        case JSON::numtype::INT16: {
                            os << val.cget<int16_t>();
                        } break;
                        case JSON::numtype::UINT8: {
                            os << val.cget<uint8_t>();
                        } break;
                        case JSON::numtype::INT8: {
                            os << val.cget<int8_t>();
                        } break;
                    }
                } break;
                case value_t::OBJECT: {
                    os << "{";
                    // for(auto it = val.cget<JSON::object_t>().begin(); it != val.cget<JSON::object_t>().end();)
                    // {
                    //     os << "\"" << it->first << "\" : " << it->second;
                    //     it++;
                    //     if(it != val.cget<JSON::object_t>().end()) os << ", ";
                    // }
                    const JSON::object_t &obj = val.cget<JSON::object_t>();
                    for(auto it = obj.keys.begin(); it != obj.keys.end();)
                    {
                        os << "\"" << it->first << "\" : " << obj.values[it->second];
                        it++;
                        if(it != obj.keys.end()) os << ", ";
                    }
                    os << "}";
                } break;
                case value_t::ARRAY: {
                    os << "[";
                    for(auto it = val.cget<JSON::array_t>().begin(); it != val.cget<JSON::array_t>().end();)
                    {
                        os << *it;
                        it++;
                        if(it != val.cget<JSON::array_t>().end()) os << ", ";
                    }
                    os << "]";
                } break;
            }
            return os;
        }

        istream& operator>>(istream& is, value_t& val)
        {
            skipAll(is, LAK_JSON_WHITESPACE);
            switch(is.peek())
            {
                case '{': {
                    JSON::object_t& obj = val.setType<JSON::OBJECT>();
                    obj.clear();
                    // skipOne(is, "\"");
                    skipNone(is, "\'\"}");
                    istream::char_type c;// = is.get();
                    while((c = is.peek()) != '}')
                    {
                        if(c == ',')
                        {
                            is.get();
                            skipAll(is, LAK_JSON_WHITESPACE);
                        }
                        JSON::string_t str = getString<JSON::string_t::value_type, istream::char_type>(is);
                        skipOne(is, ":");
                        value_t v;
                        is >> v;
                        obj[str] = v;
                        skipAll(is, LAK_JSON_WHITESPACE);
                    }
                    is.get(); // remove }
                } break;

                case '[': {
                    JSON::array_t& arr = val.setType<JSON::ARRAY>();
                    arr.clear();
                    char c = is.get();
                    while((c = is.peek()) != ']')
                    {
                        if(c == ',') is.get();
                        value_t v;
                        is >> v;
                        arr.push_back(v);
                        skipAll(is, LAK_JSON_WHITESPACE);
                    }
                    is.get(); // remove ]
                } break;

                case '\'':
                case '\"': {
                    JSON::string_t& str = val.setType<JSON::STRING>();
                    str = getString<JSON::string_t::value_type, istream::char_type>(is);
                } break;

                case 'T':
                case 't': {
                    bool& b = val.setType<JSON::BOOL>();
                    b = true;
                    skipAll(is, "true");
                } break;

                case 'F':
                case 'f': {
                    bool& b = val.setType<JSON::BOOL>();
                    b = false;
                    skipAll(is, "false");
                } break;

                case 'N':
                case 'n': {
                    val.setType<JSON::JNULL>();
                    skipAll(is, "nul");
                } break;

                default: { // assume number
                    JSON::number_t& num = val.setType<JSON::NUMBER>();
                    skipAll(is, LAK_JSON_WHITESPACE);
                    // getNumberT(long double, is, num);
                    getNumberT(double, is, num);
                    // is >> std::get<double>(num);
                } break;
            }
            return is;
        }
    }
}