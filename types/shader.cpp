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

#include "types/shader.h"

namespace lak
{
    shaderProgram_t::shaderProgram_t(bool autoinit) : program(autoinit ? glCreateProgram() : 0) {}
    shaderProgram_t::shaderProgram_t(GLuint other) : program(other) {}
    shaderProgram_t::shaderProgram_t(shaderProgram_t &&other) : program(other.program), attributes(other.attributes), uniforms(other.uniforms) { other.program = 0; }
    shaderProgram_t::~shaderProgram_t() { if (program) glDeleteProgram(program); }

    bool shaderProgram_t::init()
    {
        program = glCreateProgram();
        LASSERT(program, "Failed to create shader program");
        return program != 0;
    }

    bool shaderProgram_t::attachShader(string &&code, GLenum type, string *error)
    {
        return attachShader(code, type, error);
    }

    bool shaderProgram_t::attachShader(const string &code, GLenum type, string *error)
    {
        GLuint shader = glCreateShader(type);
        LASSERT(shader, "Failed to create shader");
        GLchar* shaderstr = (GLchar*)&(code.c_str()[0]);
        glShaderSource(shader, 1, &shaderstr, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            if (error)
            {
                GLint msgSize = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &msgSize);
                error->resize(msgSize);
                glGetShaderInfoLog(shader, msgSize, NULL, (GLchar*)&(error->c_str()[0]));
            } else LASSERT(false, "Failed to compile shader");
            return false;
        }
        glAttachShader(program, shader);
        return true;
    }

    bool shaderProgram_t::link(string *error)
    {
        glLinkProgram(program);
        if (!linked())
        {
            if (error)
            {
                GLint msgSize = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msgSize);
                error->resize(msgSize);
                glGetProgramInfoLog(program, msgSize, NULL, (GLchar*)&(error->c_str()[0]));
            } else LASSERT(false, "Failed to link program");
            return false;
        }

        // link was successful, so initialise attributes and uniforms
        glUseProgram(program);

        GLint count;
        vector<GLchar> name;

        GLint nameLen = 0;
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameLen);
        name.resize(nameLen+1);

        attributes.clear();
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
        for (GLint i = 0; i < count; ++i)
        {
            memset(&name[0], 0, name.size());
            shaderAttribute_t attribute;
            GLenum type;
            GLint size;
            glGetActiveAttrib(program, (GLuint)i, nameLen, NULL, &size, &type, &name[0]);
            switch (type)
            {
                case GL_FLOAT: attribute.type = GL_FLOAT; attribute.size = size; break;
                case GL_FLOAT_VEC2: attribute.type = GL_FLOAT; attribute.size = size * 2; break;
                case GL_FLOAT_VEC3: attribute.type = GL_FLOAT; attribute.size = size * 3; break;
                case GL_FLOAT_VEC4: // same as MAT2
                case GL_FLOAT_MAT2: attribute.type = GL_FLOAT; attribute.size = size * 4; break;
                case GL_FLOAT_MAT3: attribute.type = GL_FLOAT; attribute.size = size * 9; break;
                case GL_FLOAT_MAT4: attribute.type = GL_FLOAT; attribute.size = size * 16; break;
                default: attribute.type = NULL; attribute.size = NULL; break;
            }
            attribute.position = glGetAttribLocation(program, &name[0]);
            attribute.name = &name[0];
            attributes[attribute.name] = attribute;
        }

        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameLen);
        name.resize(nameLen+1);

        uniforms.clear();
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
        for (GLint i = 0; i < count; ++i)
        {
            memset(&name[0], 0, name.size());
            shaderAttribute_t attribute;
            glGetActiveUniform(program, (GLuint)i, nameLen, NULL, &attribute.size, &attribute.type, &name[0]);
            attribute.position = glGetUniformLocation(program, &name[0]);
            attribute.name = &name[0];
            uniforms[attribute.name] = attribute;
        }

        return true;
    }

    bool shaderProgram_t::linked()
    {
        GLint linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        return linked != 0;
    }

    bool shaderProgram_t::validate(string *message)
    {
        glValidateProgram(program);
        GLint valid = 0;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &valid);
        if (message)
        {
            GLint msgSize = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msgSize);
            message->resize(msgSize);
            glGetProgramInfoLog(program, msgSize, NULL, (GLchar*)&(message->c_str()[0]));
        } else LASSERT(valid, "Failed to validate shader");
        return valid != 0;
    }

    void shaderProgram_t::setUniform(const string& name, const void* data, GLsizei size, GLboolean transpose)
    {
        auto uniform = uniforms.find(name);
        if (uniform == uniforms.end()) {
            #ifdef LTEST
            LERROR("Failed to find uniform \"" << name << "\"");
            LERROR("Available uniforms are :");
            for (auto it : uniforms) LERROR(it.first);
            #endif
            return;
        }
        enable();
        shaderAttribute_t &attribute = uniform->second;
        if(attribute.position < 0) return;
        switch (attribute.type)
        {
            case GL_FLOAT:
                glUniform1fv(attribute.position, size, (GLfloat*)data); break;
            case GL_FLOAT_VEC2:
                glUniform2fv(attribute.position, size, (GLfloat*)data); break;
            case GL_FLOAT_VEC3:
                glUniform3fv(attribute.position, size, (GLfloat*)data); break;
            case GL_FLOAT_VEC4:
                glUniform4fv(attribute.position, size, (GLfloat*)data); break;
            case GL_FLOAT_MAT2:
                glUniformMatrix2fv(attribute.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT2x3:
                glUniformMatrix2x3fv(attribute.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT2x4:
                glUniformMatrix2x4fv(attribute.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT3:
                glUniformMatrix3fv(attribute.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT3x2:
                glUniformMatrix3x2fv(attribute.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT3x4:
                glUniformMatrix3x4fv(attribute.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(attribute.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT4x2:
                glUniformMatrix4x2fv(attribute.position, size, transpose, (GLfloat*)data); break;
            case GL_FLOAT_MAT4x3:
                glUniformMatrix4x3fv(attribute.position, size, transpose, (GLfloat*)data); break;
            case GL_DOUBLE:
                glUniform1dv(attribute.position, size, (GLdouble*)data); break;
            case GL_DOUBLE_VEC2:
                glUniform2dv(attribute.position, size, (GLdouble*)data); break;
            case GL_DOUBLE_VEC3:
                glUniform3dv(attribute.position, size, (GLdouble*)data); break;
            case GL_DOUBLE_VEC4:
                glUniform4dv(attribute.position, size, (GLdouble*)data); break;
            case GL_DOUBLE_MAT2:
                glUniformMatrix2dv(attribute.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT2x3:
                glUniformMatrix2x3dv(attribute.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT2x4:
                glUniformMatrix2x4dv(attribute.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT3:
                glUniformMatrix3dv(attribute.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT3x2:
                glUniformMatrix3x2dv(attribute.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT3x4:
                glUniformMatrix3x4dv(attribute.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT4:
                glUniformMatrix4dv(attribute.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT4x2:
                glUniformMatrix4x2dv(attribute.position, size, transpose, (GLdouble*)data); break;
            case GL_DOUBLE_MAT4x3:
                glUniformMatrix4x3dv(attribute.position, size, transpose, (GLdouble*)data); break;
            case GL_INT:
                glUniform1iv(attribute.position, size, (GLint*)data); break;
            case GL_INT_VEC2:
                glUniform2iv(attribute.position, size, (GLint*)data); break;
            case GL_INT_VEC3:
                glUniform3iv(attribute.position, size, (GLint*)data); break;
            case GL_INT_VEC4:
                glUniform4iv(attribute.position, size, (GLint*)data); break;
            case GL_UNSIGNED_INT:
                glUniform1uiv(attribute.position, size, (GLuint*)data); break;
            case GL_UNSIGNED_INT_VEC2:
                glUniform2uiv(attribute.position, size, (GLuint*)data); break;
            case GL_UNSIGNED_INT_VEC3:
                glUniform3uiv(attribute.position, size, (GLuint*)data); break;
            case GL_UNSIGNED_INT_VEC4:
                glUniform4uiv(attribute.position, size, (GLuint*)data); break;
            default: break;
        }
    }

    void shaderProgram_t::enable()
    {
        GLint curProg;
        glGetIntegerv(GL_CURRENT_PROGRAM, &curProg);
        if (curProg != program)
            glUseProgram(program);

        #ifdef LTEST
        glGetIntegerv(GL_CURRENT_PROGRAM, &curProg);
        LASSERT(curProg == program, "Failed to enable shader");
        #endif
    }

    shaderProgram_t operator"" _shaderProgram (const char *str, const size_t size)
    {
        string _str = str;
        size_t vshaderpos = _str.find("#VERTEX_SHADER");
        size_t fshaderpos = _str.find("#FRAGMENT_SHADER");
        shaderProgram_t shader(true);

        if (vshaderpos < fshaderpos)
        {
            vshaderpos = _str.find_first_of("\n", vshaderpos);
            LASSERT(shader.attachShader(_str.substr(vshaderpos, fshaderpos - vshaderpos), GL_VERTEX_SHADER),
                " Failed to attach vertex shader");
            fshaderpos = _str.find_first_of("\n", fshaderpos);
            LASSERT(shader.attachShader(_str.substr(fshaderpos), GL_FRAGMENT_SHADER),
                " Failed to attach fragment shader");
        }
        else
        {
            fshaderpos = _str.find_first_of("\n", fshaderpos);
            LASSERT(shader.attachShader(_str.substr(fshaderpos, vshaderpos - fshaderpos), GL_FRAGMENT_SHADER),
                " Failed to attach fragment shader");
            vshaderpos = _str.find_first_of("\n", vshaderpos);
            LASSERT(shader.attachShader(_str.substr(vshaderpos), GL_VERTEX_SHADER),
                " Failed to attach vertex shader");
        }

        LASSERT(shader.link(), " Failed to link shader");
        LASSERT(shader.validate(), " Failed to validate shader");

        return shader;
    }

    shader_t operator"" _shader(const char *str, const size_t size)
    {
        return std::make_shared<lak::shaderProgram_t>(operator"" _shaderProgram(str, size));
    }
}