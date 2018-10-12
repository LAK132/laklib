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

#include "utils/stream_util.h"

void readFile(const string &src, string *dst)
{
    *dst = "";
    ifstream strm(src.c_str(), std::ios::binary | ifstream::in);
    if (!strm.is_open())
    {
        LDEBUG("Failed to open file " << src.c_str());
        return;
    }
    streampos start = strm.tellg();
    size_t size = 0;
    string str = "";
    str += EOF;
    skipOneS(strm, str)++ size; // I'm not sorry
    strm.clear();
    strm.seekg(start);
    dst->reserve(size);
    for (char c = strm.get(); strm >> c;)
        *dst += c;
}

void readFile(string &&src, string *dst)
{
    readFile(src, dst);
}

string readFile(const string &src)
{
    string str;
    readFile(src, &str);
    return str;
}

string readFile(string &&src)
{
    string str;
    readFile(src, &str);
    return str;
}