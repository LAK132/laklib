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
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#ifndef LAK_GL_INCLUDE
#define LAK_GL_INCLUDE <GL/gl3w.h>
#endif
#include LAK_GL_INCLUDE

#include "utils/ldebug.h"
#include "types/shader.h"
#include "types/texture.h"
#include "types/stride_vector.h"

#ifndef LAK_MESH_H
#define LAK_MESH_H

namespace lak
{
    using std::vector;
    using std::unordered_map;
    using std::string;
    using std::shared_ptr;

    struct vertexElement_t
    {
        // readonly
        size_t interlacedStride = 0;// data stride once interlaced in the vertex buffer
        GLintptr offset = 0;        // buffer position
        // params
        stride_vector data = {};    // data to push to the GPU
        GLenum type = GL_FLOAT;     // type of the data
        GLint size = 4;             // `type`s per element count (vec2 = 2, vec3 = 3, etc...)
        GLuint divisor = 0;         // buffer steps per vertex (0->every vtx, 1->every mesh, 2->every 2 meshes, etc...)
        bool normalised = false;
        bool active = false;
        bool dirty = true;          // set to true to update during the next update call

        // stringable functions to initalise vertex element
        template<typename T> inline vertexElement_t& setData(GLenum t, GLint s, const T &d)
        { dirty = true; type = t; size = s; data = d; return *this; }
        template<typename T> inline vertexElement_t& setData(GLenum t, GLint s, T &&d)
        { dirty = true; type = t; size = s; data = d; return *this; }
        inline vertexElement_t& setDivisor(GLuint d) { divisor = d; return *this; }
        inline vertexElement_t& setNormalised(bool n) { normalised = n; return *this; }
        inline vertexElement_t& setActive(bool n) { active = n; return *this; }
    };

    struct vertexBuffer_t
    {
        // readonly
        bool init = false;
        GLuint buffer = 0xBADBEEF;
        size_t size = 0;
        // params
        unordered_map<string, vertexElement_t> elements{};
        GLenum usage = GL_STATIC_DRAW;
        vertexBuffer_t();
        ~vertexBuffer_t();
        void bind();
        bool update();
    };

    struct vertexArray_t
    {
        // readonly
        GLuint vertArray = 0;
        GLuint indexBuffer = 0;
        // params
        vector<vertexBuffer_t> buffers = {};
        void init();
        void bind();
        ~vertexArray_t();
    };

    struct mesh_t
    {
        // readonly
        vertexArray_t vertArray;
        // params
        shader_t shader;
        unordered_map<string, shared_ptr<texture_t>> textures{};
        vector<GLuint> index = {};  // will use index mode if size > 0
        GLenum drawMode = GL_TRIANGLES;
        size_t indexCount = 0;      // readonly if using index
        bool dirty = true;          // set true to force an update during the next draw call
        void update();
        void draw(GLsizei count = 1, const void *indexOffset = NULL);
    };
}

#endif // LAK_MESH_H