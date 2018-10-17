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

#include "types/mesh.h"

namespace lak
{
    //
    // vertexBuffer_t
    //

    vertexBuffer_t::vertexBuffer_t() { glGenBuffers(1, &buffer); }
    vertexBuffer_t::~vertexBuffer_t() { glDeleteBuffers(1, &buffer); }

    void vertexBuffer_t::bind()
    {
        GLint current;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current);
        if (current != buffer) glBindBuffer(GL_ARRAY_BUFFER, buffer);
    }

    bool vertexBuffer_t::update()
    {
        bind();

        vector<stride_vector*> groupElems = {};
        size_t groupStride = 0;

        bool dirty = false;
        for (auto &elem : elements)
            if (elem.second.dirty) { dirty = true; break; }

        if (dirty)
        {
            groupElems.reserve(elements.size());
            for (auto &elem : elements)
            {
                if (!elem.second.active || !elem.second.data.stride) continue;
                elem.second.dirty = false;
                elem.second.offset = groupStride;
                groupElems.push_back(&(elem.second.data));
                groupStride += elem.second.data.stride;
            }

            for (auto &elem : elements)
                elem.second.interlacedStride = groupStride;

            stride_vector interlaced = stride_vector::interleave(groupElems);

            if (interlaced.size() == size)
            {
                glBufferSubData(GL_ARRAY_BUFFER, NULL, size, &interlaced.data[0]);
            }
            else
            {
                size = interlaced.size();
                glBufferData(GL_ARRAY_BUFFER, size, &interlaced.data[0], usage);
            }
        }

        return dirty;
    }

    //
    // vertexArray_t
    //

    vertexArray_t::~vertexArray_t()
    {
        if (!vertArray) return;
        glDeleteVertexArrays(1, &vertArray);
        glDeleteBuffers(1, &indexBuffer);
    }

    void vertexArray_t::init()
    {
        if (vertArray) return;
        glGenVertexArrays(1, &vertArray);
        glGenBuffers(1, &indexBuffer);
    }

    void vertexArray_t::bind()
    {
        GLint current;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
        if (current != vertArray) glBindVertexArray(vertArray);
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &current);
        if (current != indexBuffer) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    }

    //
    // mesh_t
    //

    void mesh_t::update()
    {
        if (!vertArray.vertArray)
            vertArray.init();
        vertArray.bind();

        for (auto &buffer : vertArray.buffers)
        {
            if (buffer.update() && shader.use_count() > 0)
            for (auto &element : buffer.elements)
            {
                auto &&attribute = shader->attributes.find(element.first);
                if (attribute != shader->attributes.end())
                {
                    if (element.second.active)
                    {
                        glEnableVertexAttribArray(attribute->second.position);
                        glVertexAttribDivisor(attribute->second.position, element.second.divisor);
                        glVertexAttribPointer(attribute->second.position, attribute->second.size, attribute->second.type, element.second.normalised, element.second.interlacedStride, (GLvoid*)element.second.offset);
                        // glVertexAttribPointer(attribute->second.position, element.second.size, element.second.type, element.second.normalised, element.second.interlacedStride, (GLvoid*)element.second.offset);
                        attribute->second.active = true;
                    }
                    else
                    {
                        glDisableVertexAttribArray(attribute->second.position);
                        attribute->second.active = false;
                    }
                }
            }
        }

        if (index.size() > 0)
        {
            indexCount = index.size();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), &(index[0]), GL_STATIC_DRAW);
        }

        dirty = false;
    }

    void mesh_t::draw(GLsizei count, const void *indexOffset)
    {
        if (dirty) update();
        else vertArray.bind();
        if (shader.use_count() > 0)
            shader->enable();
        for (auto &texture : textures)
            if (texture.second.use_count() > 0)
                texture.second->bind();
        if (index.size() > 0)
            glDrawElementsInstanced(drawMode, indexCount, GL_UNSIGNED_INT, indexOffset, count);
        else
            glDrawArraysInstanced(drawMode, 0, indexCount, count);
    }
}