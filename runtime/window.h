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
#include <cassert>
#include <string>
#include <thread>

#include <SDL.h>

#include "types/queue.h"
#include "runtime/mainloop.h"

#ifndef LAK_WINDOW_NO_DEFINES
#define withWindowContext(window, context) if (lak::window_t::context_t context(window); context)
#endif // LAK_WINDOW_NO_DEFINES

#ifndef LAK_WINDOW_H
#define LAK_WINDOW_H

namespace lak
{
    using std::vector;
    using std::string;
    using std::thread;

    struct view_t
    {
        // pixel count
        size_t w;
        size_t h;
        // screen size
        float x;
        float y;
    };

    struct window_t
    {
        struct context_t
        {
            window_t &manager;
            ticket_t ticket;
            context_t(window_t &wm);
            ~context_t();
            operator bool();
        };
        string name = "OpenGL";
        SDL_Window *window = nullptr;
        SDL_DisplayMode mode;
        SDL_GLContext context;
        queue_t queue;
        view_t view;

        void open(int displayNumber = 0);
        void close();
        ~window_t();
    };
}

#endif // LAK_WINDOW_H