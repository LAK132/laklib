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

#include <stdint.h>
#include <cstddef>
#include <vector>
#include <memory>
#include <cstring>

#ifndef LAK_STRIDE_VECTOR_H
#define LAK_STRIDE_VECTOR_H

namespace lak
{
    using std::vector;
    using std::memcpy;
    struct stride_vector
    {
        size_t stride = 1;
        vector<uint8_t> data;
        stride_vector();
        stride_vector(size_t size);
        stride_vector(const stride_vector& other);
        stride_vector(stride_vector&& other);
        void init(size_t size, size_t str = 1);
        inline size_t size() const { return data.size(); }
        stride_vector& operator=(const stride_vector& other);
        stride_vector& operator=(stride_vector&& other);
        vector<uint8_t> operator[](size_t idx) const;
        template <typename T>
        T& at(size_t idx)
        {
            return *static_cast<T*>(&data[sizeof(T) * idx]);
        }
        template <typename T>
        const T& at(size_t idx) const
        {
            return *static_cast<T*>(&data[sizeof(T) * idx]);
        }
        template <typename T>
        stride_vector& operator=(const vector<T>& other)
        {
            stride = sizeof(T);
            data.resize(sizeof(T) * other.size());
            memcpy(&data[0], &other[0], data.size());
            return *this;
        }
        template <typename T>
        stride_vector& operator=(vector<T>&& other)
        {
            stride = sizeof(T);
            data.resize(sizeof(T) * other.size());
            memcpy(&data[0], &other[0], data.size());
            return *this;
        }
        template <typename T>
        inline T* get()
        {
            return (T*)&(data[0]);
        }
        template <typename T>
        static stride_vector strideify(const vector<T> &other)
        {
            stride_vector rtn;
            return rtn = other;
        }
        template <typename T>
        static stride_vector strideify(vector<T>&& other)
        {
            stride_vector rtn;
            return rtn = other;
        }
        static stride_vector interleave(const vector<stride_vector*> &vecs);
        static inline stride_vector interleave(vector<stride_vector*> &&vecs) { return interleave(vecs); };
    };
}

#ifdef LAK_STRIDE_VECTOR_IMPLEM
#   ifndef LAK_STRIDE_VECTOR_HAS_IMPLEM
#       define LAK_STRIDE_VECTOR_HAS_IMPLEM
#       include "types/stride_vector.cpp"
#   endif // LAK_STRIDE_VECTOR_HAS_IMPLEM
#endif // LAK_STRIDE_VECTOR_IMPLEM

#endif // LAK_STRIDE_VECTOR_H