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

#include "runtime/window.h"

namespace lak
{
    window_t::context_t::context_t(window_t &wm) : manager(wm)
    {
        ticket = manager.queue.lock();
        SDL_GL_MakeCurrent(manager.window, manager.context);
    }

    window_t::context_t::~context_t()
    {
        if (ticket)
            SDL_GL_MakeCurrent(manager.window, 0);
        ticket = nullptr;
    }

    window_t::context_t::operator bool()
    {
        return ticket != nullptr;
    }

    void window_t::open(int displayNumber)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) { assert(false); throw; }

        view.w = 1280;
        view.h = 720;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        SDL_GetCurrentDisplayMode(displayNumber, &mode);
        window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, view.w, view.h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        context = SDL_GL_CreateContext(window);

        SDL_GL_MakeCurrent(window, context);

        if (SDL_GL_SetSwapInterval(-1) == -1)
            SDL_GL_SetSwapInterval(1);

        // SDL_GL_MakeCurrent(window, 0);
    }

    window_t::~window_t()
    {
        if (window)
            close();
    }

    void window_t::close()
    {
        SDL_GL_MakeCurrent(window, context);
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}