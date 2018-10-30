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
#include <type_traits>

#ifndef LAK_GL_INCLUDE
#define LAK_GL_INCLUDE <GL/gl3w.h>
#endif
#include LAK_GL_INCLUDE

#ifndef LAK_COLOR
#define LAK_COLOR

namespace lak
{
    using std::conditional;
    using std::enable_if;
    using std::is_same;

    template<GLenum gltype> struct gl_color_1 { typedef nullptr_t type; };
    template<> struct gl_color_1<GL_UNSIGNED_BYTE> { typedef uint8_t type; };
    template<> struct gl_color_1<GL_BYTE> { typedef int8_t type; };
    template<> struct gl_color_1<GL_UNSIGNED_SHORT> { typedef uint16_t type; };
    template<> struct gl_color_1<GL_SHORT> { typedef int16_t type; };
    template<> struct gl_color_1<GL_UNSIGNED_INT> { typedef uint32_t type; };
    template<> struct gl_color_1<GL_INT> { typedef int32_t type; };
    template<> struct gl_color_1<GL_FLOAT> { typedef float type; };
    template<GLenum gltype> using gl_color_1_t = typename gl_color_1<gltype>::type;

    template<GLenum gltype> struct gl_color_3 { typedef nullptr_t type; };
    template<> struct gl_color_3<GL_UNSIGNED_BYTE_3_3_2> { typedef uint8_t type; };
    template<> struct gl_color_3<GL_UNSIGNED_BYTE_2_3_3_REV> { typedef uint8_t type; };
    template<> struct gl_color_3<GL_UNSIGNED_SHORT_5_6_5> { typedef uint16_t type; };
    template<> struct gl_color_3<GL_UNSIGNED_SHORT_5_6_5_REV> { typedef uint16_t type; };
    template<GLenum gltype> using gl_color_3_t = typename gl_color_3<gltype>::type;

    template<GLenum gltype> struct gl_color_4 { typedef nullptr_t type; };
    template<> struct gl_color_4<GL_UNSIGNED_SHORT_4_4_4_4> { typedef uint16_t type; };
    template<> struct gl_color_4<GL_UNSIGNED_SHORT_4_4_4_4_REV> { typedef uint16_t type; };
    template<> struct gl_color_4<GL_UNSIGNED_SHORT_5_5_5_1> { typedef uint16_t type; };
    template<> struct gl_color_4<GL_UNSIGNED_SHORT_1_5_5_5_REV> { typedef uint16_t type; };
    template<> struct gl_color_4<GL_UNSIGNED_INT_8_8_8_8> { typedef uint32_t type; };
    template<> struct gl_color_4<GL_UNSIGNED_INT_8_8_8_8_REV> { typedef uint32_t type; };
    template<> struct gl_color_4<GL_UNSIGNED_INT_10_10_10_2> { typedef uint32_t type; };
    template<> struct gl_color_4<GL_UNSIGNED_INT_2_10_10_10_REV> { typedef uint32_t type; };
    template<GLenum gltype> using gl_color_4_t = typename gl_color_4<gltype>::type;

    template<GLenum gltype, typename T=void>struct gl_color;
    template<GLenum gltype> struct gl_color<gltype, typename enable_if<!is_same<gl_color_1_t<gltype>, nullptr_t>::value>::type>
    { using type = gl_color_1<gltype>; };
    // { using type = gl_color_1_t<gltype>; };
    template<GLenum gltype> struct gl_color<gltype, typename enable_if<!is_same<gl_color_3_t<gltype>, nullptr_t>::value>::type>
    { using type = gl_color_3<gltype>; };
    // { using type = gl_color_3_t<gltype>; };
    template<GLenum gltype> struct gl_color<gltype, typename enable_if<!is_same<gl_color_4_t<gltype>, nullptr_t>::value>::type>
    { using type = gl_color_4<gltype>; };
    // { using type = gl_color_4_t<gltype>; };
    template<GLenum gltype> using gl_color_t = typename gl_color<gltype>::type::type;

    template<GLenum glformat, GLenum gltype, typename T> struct color;
    // RX
    template<GLenum gltype>
    struct color<GL_RED, gltype, gl_color_1<gltype>>
    {
        using type_t = color<GL_RED, gltype, gl_color_1<gltype>>;
        using ctype_t = gl_color_t<gltype>;
        ctype_t r = 0;
        color() {}
        color(ctype_t R) : r(R) {}
        color(ctype_t *c) : r(c[0]) {}
        color(type_t&& other) { *this = other; }
        color(const type_t& other) { *this = other; }
        type_t& operator=(ctype_t *c) { return *this = type_t(c); }
        type_t& operator=(type_t&& other) { return *this = other; }
        type_t& operator=(const type_t& other) {
            r = other.r;
            return *this;
        }
    };
    // RX GX
    template<GLenum gltype>
    struct color<GL_RG, gltype, gl_color_1<gltype>>
    {
        using type_t = color<GL_RG, gltype, gl_color_1<gltype>>;
        using ctype_t = gl_color_t<gltype>;
        ctype_t r = 0; ctype_t g = 0;
        color() {}
        color(ctype_t R, ctype_t G) : r(R), g(G) {}
        color(ctype_t *c) : r(c[0]), g(c[1]) {}
        color(type_t&& other) { *this = other; }
        color(const type_t& other) { *this = other; }
        type_t& operator=(ctype_t *c) { return *this = type_t(c); }
        type_t& operator=(type_t&& other) { return *this = other; }
        type_t& operator=(const type_t& other) {
            r = other.r;
            g = other.g;
            return *this;
        }
    };
    // RX GX BX
    template<GLenum gltype>
    struct color<GL_RGB, gltype, gl_color_1<gltype>>
    {
        using type_t = color<GL_RGB, gltype, gl_color_1<gltype>>;
        using ctype_t = gl_color_t<gltype>;
        ctype_t r = 0; ctype_t g = 0; ctype_t b = 0;
        color() {}
        color(ctype_t R, ctype_t G, ctype_t B) : r(R), g(G), b(B) {}
        color(ctype_t *c) : r(c[0]), g(c[1]), b(c[2]) {}
        color(type_t&& other) { *this = other; }
        color(const type_t& other) { *this = other; }
        type_t& operator=(ctype_t *c) { return *this = type_t(c); }
        type_t& operator=(type_t&& other) { return *this = other; }
        type_t& operator=(const type_t& other) {
            r = other.r;
            g = other.g;
            b = other.b;
            return *this;
        }
    };
    // BX GX RX
    template<GLenum gltype>
    struct color<GL_BGR, gltype, gl_color_1<gltype>>
    {
        using type_t = color<GL_BGR, gltype, gl_color_1<gltype>>;
        using ctype_t = gl_color_t<gltype>;
        ctype_t b = 0; ctype_t g = 0; ctype_t r = 0;
        color() {}
        color(ctype_t B, ctype_t G, ctype_t R) : r(R), g(G), b(B) {}
        color(ctype_t *c) : r(c[2]), g(c[1]), b(c[0]) {}
        color(type_t&& other) { *this = other; }
        color(const type_t& other) { *this = other; }
        type_t& operator=(ctype_t *c) { return *this = type_t(c); }
        type_t& operator=(type_t&& other) { return *this = other; }
        type_t& operator=(const type_t& other) {
            r = other.r;
            g = other.g;
            b = other.b;
            return *this;
        }
    };
    // RX GX BX AX
    template<GLenum gltype>
    struct color<GL_RGBA, gltype, gl_color_1<gltype>>
    {
        using type_t = color<GL_RGBA, gltype, gl_color_1<gltype>>;
        using ctype_t = gl_color_t<gltype>;
        ctype_t r = 0; ctype_t g = 0; ctype_t b = 0; ctype_t a = 0;
        color() {}
        color(ctype_t R, ctype_t G, ctype_t B, ctype_t A) : r(R), g(G), b(B), a(A) {}
        color(ctype_t *c) : r(c[0]), g(c[1]), b(c[2]), a(c[3]) {}
        color(type_t&& other) { *this = other; }
        color(const type_t& other) { *this = other; }
        type_t& operator=(ctype_t *c) { return *this = type_t(c); }
        type_t& operator=(type_t&& other) { return *this = other; }
        type_t& operator=(const type_t& other) {
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            return *this;
        }
    };
    // BX GX RX AX
    template<GLenum gltype>
    struct color<GL_BGRA, gltype, gl_color_1<gltype>>
    {
        using type_t = color<GL_BGRA, gltype, gl_color_1<gltype>>;
        using ctype_t = gl_color_t<gltype>;
        ctype_t b = 0; ctype_t g = 0; ctype_t r = 0; ctype_t a = 0;
        color() {}
        color(ctype_t B, ctype_t G, ctype_t R, ctype_t A) : r(R), g(G), b(B), a(A) {}
        color(ctype_t *c) : r(c[2]), g(c[1]), b(c[0]), a(c[3]) {}
        color(type_t&& other) { *this = other; }
        color(const type_t& other) { *this = other; }
        type_t& operator=(ctype_t *c) { return *this = type_t(c); }
        type_t& operator=(type_t&& other) { return *this = other; }
        type_t& operator=(const type_t& other) {
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            return *this;
        }
    };

    // R3 G3 B2 / B2 G3 R3
    // R5 G6 B5 / B5 G6 R5
    template<GLenum gltype>
    struct color<GL_RGB, gltype, gl_color_3<gltype>>
    {
        using type_t = color<GL_RGB, gltype, gl_color_3<gltype>>;
        using ctype_t = gl_color_t<gltype>;
        ctype_t rgb = 0;
        color() {}
        color(ctype_t RGB) : rgb(RGB) {}
        color(ctype_t *c) : rgb(c[0]) {}
        color(type_t&& other) { *this = other; }
        color(const type_t& other) { *this = other; }
        type_t& operator=(ctype_t *c) { return *this = type_t(c); }
        type_t& operator=(type_t&& other) { return *this = other; }
        type_t& operator=(const type_t& other) {
            rgb = other.rgb;
            return *this;
        }
    };

    // R4 G4 B4 A4 / A4 B4 G4 R4
    // R5 G5 B5 A1 / A1 B5 G5 R5
    // R8 G8 B8 A8 / A8 B8 G8 R8
    // R10 G10 B10 A2 / B10 G10 R10 A2
    template<GLenum gltype>
    struct color<GL_RGBA, gltype, gl_color_4<gltype>>
    {
        using type_t = color<GL_RGBA, gltype, gl_color_4<gltype>>;
        using ctype_t = gl_color_t<gltype>;
        ctype_t rgba = 0;
        color() {}
        color(ctype_t RGBA) : rgba(RGBA) {}
        color(ctype_t *c) : rgba(c[0]) {}
        color(type_t&& other) { *this = other; }
        color(const type_t& other) { *this = other; }
        type_t& operator=(ctype_t *c) { return *this = type_t(c); }
        type_t& operator=(type_t&& other) { return *this = other; }
        type_t& operator=(const type_t& other) {
            rgba = other.rgba;
            return *this;
        }
    };

    // B4 G4 R4 A4 / A4 R4 G4 B4
    // B5 G5 R5 A1 / A1 R5 G5 B5
    // B8 G8 R8 A8 / A8 R8 G8 B8
    // B10 G10 R10 A2 / R10 G10 B10 A2
    template<GLenum gltype>
    struct color<GL_BGRA, gltype, gl_color_4<gltype>>
    {
        using type_t = color<GL_BGRA, gltype, gl_color_4<gltype>>;
        using ctype_t = gl_color_t<gltype>;
        ctype_t bgra = 0;
        color() {}
        color(ctype_t BGRA) : bgra(BGRA) {}
        color(ctype_t *c) : bgra(c[0]) {}
        color(type_t&& other) { *this = other; }
        color(const type_t& other) { *this = other; }
        type_t& operator=(ctype_t *c) { return *this = type_t(c); }
        type_t& operator=(type_t&& other) { return *this = other; }
        type_t& operator=(const type_t& other) {
            bgra = other.bgra;
            return *this;
        }
    };

    template<GLenum glformat, GLenum gltype>
    using color_t = color<glformat, gltype, typename gl_color<gltype>::type>;

    using colorR8_t = color_t<GL_RED, GL_UNSIGNED_BYTE>;
    using colorRG8_t = color_t<GL_RG, GL_UNSIGNED_BYTE>;
    using colorRGB8_t = color_t<GL_RGB, GL_UNSIGNED_BYTE>;
    using colorBGR8_t = color_t<GL_BGR, GL_UNSIGNED_BYTE>;
    using colorRGBA8_t = color_t<GL_RGBA, GL_UNSIGNED_BYTE>;
    using colorBGRA8_t = color_t<GL_BGRA, GL_UNSIGNED_BYTE>;

    using colorRs8_t = color_t<GL_RED, GL_BYTE>;
    using colorRGs8_t = color_t<GL_RG, GL_BYTE>;
    using colorRGBs8_t = color_t<GL_RGB, GL_BYTE>;
    using colorBGRs8_t = color_t<GL_BGR, GL_BYTE>;
    using colorRGBAs8_t = color_t<GL_RGBA, GL_BYTE>;
    using colorBGRAs8_t = color_t<GL_BGRA, GL_BYTE>;

    using colorR16_t = color_t<GL_RED, GL_UNSIGNED_SHORT>;
    using colorRG16_t = color_t<GL_RG, GL_UNSIGNED_SHORT>;
    using colorRGB16_t = color_t<GL_RGB, GL_UNSIGNED_SHORT>;
    using colorBGR16_t = color_t<GL_BGR, GL_UNSIGNED_SHORT>;
    using colorRGBA16_t = color_t<GL_RGBA, GL_UNSIGNED_SHORT>;
    using colorBGRA16_t = color_t<GL_BGRA, GL_UNSIGNED_SHORT>;

    using colorRs16_t = color_t<GL_RED, GL_SHORT>;
    using colorRGs16_t = color_t<GL_RG, GL_SHORT>;
    using colorRGBs16_t = color_t<GL_RGB, GL_SHORT>;
    using colorBGRs16_t = color_t<GL_BGR, GL_SHORT>;
    using colorRGBAs16_t = color_t<GL_RGBA, GL_SHORT>;
    using colorBGRAs16_t = color_t<GL_BGRA, GL_SHORT>;

    using colorR32_t = color_t<GL_RED, GL_UNSIGNED_INT>;
    using colorRG32_t = color_t<GL_RG, GL_UNSIGNED_INT>;
    using colorRGB32_t = color_t<GL_RGB, GL_UNSIGNED_INT>;
    using colorBGR32_t = color_t<GL_BGR, GL_UNSIGNED_INT>;
    using colorRGBA32_t = color_t<GL_RGBA, GL_UNSIGNED_INT>;
    using colorBGRA32_t = color_t<GL_BGRA, GL_UNSIGNED_INT>;

    using colorRs32_t = color_t<GL_RED, GL_INT>;
    using colorRGs32_t = color_t<GL_RG, GL_INT>;
    using colorRGBs32_t = color_t<GL_RGB, GL_INT>;
    using colorBGRs32_t = color_t<GL_BGR, GL_INT>;
    using colorRGBAs32_t = color_t<GL_RGBA, GL_INT>;
    using colorBGRAs32_t = color_t<GL_BGRA, GL_INT>;

    using colorRf_t = color_t<GL_RED, GL_FLOAT>;
    using colorRGf_t = color_t<GL_RG, GL_FLOAT>;
    using colorRGBf_t = color_t<GL_RGB, GL_FLOAT>;
    using colorBGRf_t = color_t<GL_BGR, GL_FLOAT>;
    using colorRGBAf_t = color_t<GL_RGBA, GL_FLOAT>;
    using colorBGRAf_t = color_t<GL_BGRA, GL_FLOAT>;

    using colorR3G3B2_t = color_t<GL_RGB, GL_UNSIGNED_BYTE_3_3_2>;
    using colorB2G3R3_t = color_t<GL_RGB, GL_UNSIGNED_BYTE_2_3_3_REV>;

    using colorR4G4B4A4_t = color_t<GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4>;
    using colorB4G4R4A4_t = color_t<GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4>;
    using colorA4B4G4R4_t = color_t<GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4_REV>;
    using colorA4R4G4B4_t = color_t<GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV>;

    using colorR5G5B5A1_t = color_t<GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1>;
    using colorB5G5R5A1_t = color_t<GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1>;
    using colorA1B5G5R5_t = color_t<GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV>;
    using colorA1R5G5B5_t = color_t<GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV>;

    using colorR8G8B8A8_t = color_t<GL_RGBA, GL_UNSIGNED_INT_8_8_8_8>;
    using colorB8G8R8A8_t = color_t<GL_BGRA, GL_UNSIGNED_INT_8_8_8_8>;
    using colorA8B8G8R8_t = color_t<GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV>;
    using colorA8R8G8B8_t = color_t<GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV>;

    using colorR10G10B10A2_t = color_t<GL_RGBA, GL_UNSIGNED_INT_10_10_10_2>;
    using colorB10G10R10A2_t = color_t<GL_BGRA, GL_UNSIGNED_INT_10_10_10_2>;
    using colorA2B10G10R10_t = color_t<GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV>;
    using colorA2R10G10B10_t = color_t<GL_BGRA, GL_UNSIGNED_INT_2_10_10_10_REV>;
}

#endif // LAK_COLOR