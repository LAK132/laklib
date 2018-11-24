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
#include <cstring>
#include <string>
#include <memory>

#ifndef LAK_GL_INCLUDE
#define LAK_GL_INCLUDE <GL/gl3w.h>
#endif
#include LAK_GL_INCLUDE

#include "utils/ldebug.h"

#ifndef LAK_SHADER_H
#define LAK_SHADER_H

namespace lak
{
    using std::vector;
    using std::unordered_map;
    using std::string;
    using std::shared_ptr;

    struct shaderAttribute_t
    {
        GLuint position = -1;   // program position
        GLenum type = GL_FLOAT; // element type
        GLint size = 4;         // element count
        string name = "";       // name in the shader
        bool active = false;    // is the element currently in use
    };

    struct shaderProgram_t
    {
        // OpenGL shader program ID
        GLuint program; // glCreateProgram returns 0 on error, so use 0 as invalid state
        unordered_map<string, shaderAttribute_t> attributes;
        unordered_map<string, shaderAttribute_t> uniforms;

        shaderProgram_t(bool autoinit = false);
        shaderProgram_t(GLuint other);
        shaderProgram_t(shaderProgram_t &&other);
        ~shaderProgram_t();
        // calls glCreateProgram
        bool init();
        // add shader code to this program
        bool attachShader(const string &code, GLenum type, string *error = nullptr);
        // links the program with OpenGL and then initialise attributes and uniforms
        bool link(string *error = nullptr);
        // checks if successfully linked
        bool linked();
        // run validation on this shader program
        bool validate(string *message = nullptr);

        void setUniform(const string &name, const void *data, GLsizei size = 1, GLboolean transpose = GL_FALSE);
        void enable();
    };

    using shader_t = shared_ptr<shaderProgram_t>;

    shaderProgram_t operator"" _shaderProgram(const char *str, const size_t size);
    shader_t operator"" _shader(const char *str, const size_t size);
}

#endif // LAK_SHADER_H