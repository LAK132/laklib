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

#include "types/stride_vector.h"

namespace lak
{
    stride_vector::stride_vector(){}

    stride_vector::stride_vector(size_t size)
    {
        init(size);
    }

    stride_vector::stride_vector(const stride_vector &other)
    {
        stride = other.stride;
        data = other.data;
    }

    stride_vector::stride_vector(stride_vector &&other)
    {
        stride = other.stride;
        data = other.data;
    }

    void stride_vector::init(size_t size, size_t str)
    {
        stride = str;
        data.resize(size);
    }

    stride_vector &stride_vector::operator=(const stride_vector &other)
    {
        stride = other.stride;
        data = other.data;
        return *this;
    }

    stride_vector &stride_vector::operator=(stride_vector &&other)
    {
        stride = other.stride;
        data = other.data;
        return *this;
    }

    vector<uint8_t> stride_vector::operator[](size_t idx) const
    {
        vector<uint8_t> rtn(size() > stride ? stride : size());
        memcpy(&rtn[0], &data[stride * idx], rtn.size());
        return rtn;
    }

    stride_vector stride_vector::interleave(const vector<stride_vector*>& vecs)
    {
        size_t size = 0;
        size_t maxlen = 0;
        for(auto it = vecs.begin(); it != vecs.end(); it++)
        {
            size += (*it)->data.size();
            if((*it)->data.size() > maxlen) maxlen = (*it)->data.size() / (*it)->stride;
        }
        stride_vector rtn(size);
        rtn.data.clear();
        for(size_t i = 0; i < maxlen; i++)
        {
            for(auto it = vecs.begin(); it != vecs.end(); it++)
            {
                const vector<uint8_t>& vec = (**it)[i];
                for(auto it2 = vec.begin(); it2 != vec.end(); it2++)
                    rtn.data.push_back(*it2);
            }
        }
        return rtn;
    }
}