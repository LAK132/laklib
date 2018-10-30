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

#include <vector>
#include <tuple>
#include <iostream>
#include <string>

#include "utils/stream.h"

#ifndef LAK_OBJ_H
#define LAK_OBJ_H

namespace lak
{
    using std::vector;
    using std::tuple;
    using std::make_tuple;
    using std::istream;
    using std::streampos;
    using std::string;

    template <typename vert_t, typename uvw_t, typename norm_t>
    void readOBJ(istream& strm, vector<vert_t>* verts, vector<uvw_t>* uvw, vector<norm_t>* normals, vector<vector<tuple<size_t, size_t, size_t>>>* indices, size_t* vertCount)
    {
        streampos start = strm.tellg();
        size_t vcount = 0, vtcount = 0, vncount = 0, fcount = 0;
        skipAll(strm, " \r\n\t");
        while (strm.good())
        {
            string command;
            strm >> command;
            if (command == "v") ++vcount;
            else if (command == "vt") ++vtcount;
            else if (command == "vn") ++vncount;
            else if (command == "f") ++fcount;
            skipOne(strm, "\n"); // skip to end of line
            skipAll(strm, " \r\n\t"); // skip to next non-whitespace character
        }
        strm.clear();
        strm.seekg(start);

        if (verts != nullptr) { verts->clear(); verts->reserve(vcount); }
        if (uvw != nullptr) { uvw->clear(); uvw->reserve(vtcount); }
        if (normals != nullptr) { normals->clear(); normals->reserve(vncount); }
        if (indices != nullptr) { indices->clear(); indices->reserve(fcount); }

        size_t vercount = 0;

        skipAll(strm, " \r\n\t");
        while (strm.good())
        {
            string command;
            strm >> command;
            if (command == "v")
            {
                if (verts != nullptr)
                {
                    vert_t v;
                    getNumber(strm, v[0]);
                    getNumber(strm, v[1]);
                    getNumber(strm, v[2]);
                    verts->push_back(v);
                }
            }
            else if (command == "vt")
            {
                if (uvw != nullptr)
                {
                    uvw_t u;
                    getNumber(strm, u[0]);
                    getNumber(strm, u[1]);
                    if (sizeof(uvw_t) >= sizeof(u[0]) * 3)
                    {
                        skipAll(strm, " \t");
                        char c;
                        if ((c = strm.peek()) != '\r' && c != '\n')
                            getNumber(strm, u[2])
                        else u[2] = 0.0f;
                    }
                    uvw->push_back(u);
                }
            }
            else if (command == "vn")
            {
                if (normals != nullptr)
                {
                    norm_t n;
                    getNumber(strm, n[0]);
                    getNumber(strm, n[1]);
                    getNumber(strm, n[2]);
                    normals->push_back(n);
                }
            }
            else if (command == "f")
            {
                if (indices != nullptr)
                {
                    indices->push_back({});
                    auto &face = indices->back();
                    face.reserve(3);
                    char c;
                    do
                    {
                        size_t v; getNumber(strm, v); --v;
                        size_t vt = 0, vn = 0;
                        skipAll(strm, " \t");
                        if (strm.peek() == '/')
                        {
                            strm.get();
                            skipAll(strm, " \t");
                            if (strm.peek() != '/')
                            {
                                getNumber(strm, vt); --vt;
                                skipAll(strm, " \t");
                            }
                            if (strm.peek() == '/')
                            {
                                strm.get();
                                getNumber(strm, vn); --vn;
                            }
                        }
                        skipAll(strm, " \t");
                        face.push_back({v, vt, vn});
                        ++vcount;
                    } while ((c = strm.peek()) != '\r' && c != '\n' && strm.good());
                    // split the face up if it happens to be an n-gon
                    while (face.size() > 3)
                    {
                        indices->push_back({});
                        auto &newface = indices->back();
                        newface.reserve(3);
                        newface.push_back(face[0]);
                        auto &&it = face.begin() + (face.size() - 2);
                        newface.push_back(*(it++));
                        newface.push_back(*(it++));
                        face.pop_back();
                    }
                }
            }
            skipOne(strm, "\n"); // skip to end of line
            skipAll(strm, " \r\n\t"); // skip to next non-whitespace character
        }
        if (vertCount != nullptr) *vertCount = vercount;
        strm.clear();
        strm.seekg(start);
    }
}

#endif // LAK_OBJ_H