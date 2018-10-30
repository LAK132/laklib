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

#ifndef LAK_GL_INCLUDE
#define LAK_GL_INCLUDE <GL/gl3w.h>
#endif
#include LAK_GL_INCLUDE

#include "utils/ldebug.h"
#include "types/color.h"

#ifndef LAK_IMAGE_H
#define LAK_IMAGE_H

namespace lak
{
    using std::vector;

    template <typename C> struct image_t;

    template <GLenum glformat, GLenum gltype>
    struct image_t<color_t<glformat, gltype>>
    {
        using col_t = color_t<glformat, gltype>;
        using chan_t = gl_color_t<gltype>;
        size_t w = 0;
        size_t h = 0;
        vector<col_t> pixels;
        image_t(){}
        ~image_t(){}
        void resize(size_t W, size_t H)
        {
            w = W;
            h = H;
            pixels.resize(w * h);
        }
        image_t(size_t W, size_t H) { resize(W, H); }
        image_t(size_t W, size_t H, chan_t* pix)
        {
            resize(W, H);
            for(size_t i = 0; i < w * h; i++)
            {
                switch(gltype)
                {
                    case GL_UNSIGNED_BYTE:
                    case GL_BYTE:
                    case GL_UNSIGNED_SHORT:
                    case GL_SHORT:
                    case GL_UNSIGNED_INT:
                    case GL_INT:
                    case GL_FLOAT: {
                        switch(glformat)
                        {
                            case GL_RED: {
                                pixels[i] = pix+i;
                            } break;
                            case GL_RG: {
                                pixels[i] = pix+(i*2);
                            } break;
                            case GL_RGB:
                            case GL_BGR: {
                                pixels[i] = pix+(i*3);
                            } break;
                            case GL_RGBA:
                            case GL_BGRA: {
                                pixels[i] = pix+(i*4);
                            } break;
                            default: LERROR("INVALID TYPE"); return;
                        }
                    } break;
                    case GL_UNSIGNED_BYTE_3_3_2:
                    case GL_UNSIGNED_BYTE_2_3_3_REV:
                    case GL_UNSIGNED_SHORT_5_6_5:
                    case GL_UNSIGNED_SHORT_5_6_5_REV:
                    case GL_UNSIGNED_SHORT_4_4_4_4:
                    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
                    case GL_UNSIGNED_SHORT_5_5_5_1:
                    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
                    case GL_UNSIGNED_INT_8_8_8_8:
                    case GL_UNSIGNED_INT_8_8_8_8_REV:
                    case GL_UNSIGNED_INT_10_10_10_2:
                    case GL_UNSIGNED_INT_2_10_10_10_REV: {
                        pixels[i] = pix+i;
                    } break;
                    default: LERROR("INVALID TYPE"); return;
                }
            }
        }
        image_t(size_t W, const vector<col_t>& pix)
        {
            pixels = pix;
            resize(W, pix.size() / W); // resize AFTER copy
        }
        image_t(size_t W, vector<col_t>&& pix)
        {
            pixels = pix;
            resize(W, pix.size() / W); // resize AFTER copy
        }
        image_t(const image_t<col_t>& other) { *this = other; }
        image_t(image_t<col_t>&& other) { *this = other; }
        image_t<col_t>& operator=(const image_t<col_t>& other)
        {
            pixels = other.pixels;
            resize(other.w, other.h); // resize AFTER copy
            return *this;
        }
        image_t<col_t>& operator=(image_t<col_t>&& other) { return *this = other; }
        col_t* operator[](size_t y) { return &(pixels[y * w]); }
    };

    using imageR8_t = image_t<colorR8_t>;
    using imageRG8_t = image_t<colorRG8_t>;
    using imageBGR8_t = image_t<colorBGR8_t>;
    using imageRGB8_t = image_t<colorRGB8_t>;
    using imageRGBA8_t = image_t<colorRGBA8_t>;
    using imageBGRA8_t = image_t<colorBGRA8_t>;

    using imageRs8_t = image_t<colorRs8_t>;
    using imageRGs8_t = image_t<colorRGs8_t>;
    using imageRGBs8_t = image_t<colorRGBs8_t>;
    using imageBGRs8_t = image_t<colorBGRs8_t>;
    using imageRGBAs8_t = image_t<colorRGBAs8_t>;
    using imageBGRAs8_t = image_t<colorBGRAs8_t>;

    using imageR16_t = image_t<colorR16_t>;
    using imageRG16_t = image_t<colorRG16_t>;
    using imageRGB16_t = image_t<colorRGB16_t>;
    using imageBGR16_t = image_t<colorBGR16_t>;
    using imageRGBA16_t = image_t<colorRGBA16_t>;
    using imageBGRA16_t = image_t<colorBGRA16_t>;

    using imageRs16_t = image_t<colorRs16_t>;
    using imageRGs16_t = image_t<colorRGs16_t>;
    using imageRGBs16_t = image_t<colorRGBs16_t>;
    using imageBGRs16_t = image_t<colorBGRs16_t>;
    using imageRGBAs16_t = image_t<colorRGBAs16_t>;
    using imageBGRAs16_t = image_t<colorBGRAs16_t>;

    using imageR32_t = image_t<colorR32_t>;
    using imageRG32_t = image_t<colorRG32_t>;
    using imageRGB32_t = image_t<colorRGB32_t>;
    using imageBGR32_t = image_t<colorBGR32_t>;
    using imageRGBA32_t = image_t<colorRGBA32_t>;
    using imageBGRA32_t = image_t<colorBGRA32_t>;

    using imageRs32_t = image_t<colorRs32_t>;
    using imageRGs32_t = image_t<colorRGs32_t>;
    using imageRGBs32_t = image_t<colorRGBs32_t>;
    using imageBGRs32_t = image_t<colorBGRs32_t>;
    using imageRGBAs32_t = image_t<colorRGBAs32_t>;
    using imageBGRAs32_t = image_t<colorBGRAs32_t>;

    using imageRf_t = image_t<colorRf_t>;
    using imageRGf_t = image_t<colorRGf_t>;
    using imageRGBf_t = image_t<colorRGBf_t>;
    using imageBGRf_t = image_t<colorBGRf_t>;
    using imageRGBAf_t = image_t<colorRGBAf_t>;
    using imageBGRAf_t = image_t<colorBGRAf_t>;

    using imageR3G3B2_t = image_t<colorR3G3B2_t>;
    using imageB2G3R3_t = image_t<colorB2G3R3_t>;

    using imageR4G4B4A4_t = image_t<colorR4G4B4A4_t>;
    using imageB4G4R4A4_t = image_t<colorB4G4R4A4_t>;
    using imageA4B4G4R4_t = image_t<colorA4B4G4R4_t>;
    using imageA4R4G4B4_t = image_t<colorA4R4G4B4_t>;

    using imageR5G5B5A1_t = image_t<colorR5G5B5A1_t>;
    using imageB5G5R5A1_t = image_t<colorB5G5R5A1_t>;
    using imageA1B5G5R5_t = image_t<colorA1B5G5R5_t>;
    using imageA1R5G5B5_t = image_t<colorA1R5G5B5_t>;

    using imageR8G8B8A8_t = image_t<colorR8G8B8A8_t>;
    using imageB8G8R8A8_t = image_t<colorB8G8R8A8_t>;
    using imageA8B8G8R8_t = image_t<colorA8B8G8R8_t>;
    using imageA8R8G8B8_t = image_t<colorA8R8G8B8_t>;

    using imageR10G10B10A2_t = image_t<colorR10G10B10A2_t>;
    using imageB10G10R10A2_t = image_t<colorB10G10R10A2_t>;
    using imageA2B10G10R10_t = image_t<colorA2B10G10R10_t>;
    using imageA2R10G10B10_t = image_t<colorA2R10G10B10_t>;
}

#endif // LAK_IMAGE_H