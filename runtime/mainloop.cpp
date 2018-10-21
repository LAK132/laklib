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

#include <assert.h>
#include "runtime/mainloop.h"

namespace lak
{
    thread_local threadData_t threadData;

    void update_loop(loopData_t &ld)
    {
        #ifdef LAK_USE_SDL
        uint64_t &LAST = ld.updateTime;
        uint64_t NOW = SDL_GetPerformanceCounter();

        ld.updateDelta = (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
        LAST = NOW;
        #endif // LAK_USE_SDL

        #ifdef LAK_USE_MULTITHREAD
        #ifdef LAK_USE_SDL
        ticket_t eventt = ld.eventq.lock();
        ticket_t ticket;
        if(threadData.wantContext) // slower but allows for OpenGL calls
        {
            ticket = ld.windowq.lock();
            SDL_GL_MakeCurrent(ld.window, ld.context);
            threadData.haveContext = true;
        }
        else // faster but thread can't make OpenGL calls
        {
            ticket = ld.drawq.lock();
        }
        #endif // LAK_USE_SDL
        #endif // LAK_USE_MULTITHREAD

        update(ld);

        #ifdef LAK_USE_MULTITHREAD
        #ifdef LAK_USE_SDL
        if(threadData.haveContext)
        {
            SDL_GL_MakeCurrent(ld.window, 0);
            threadData.haveContext = false;
        }
        ticket = nullptr;
        eventt = nullptr;
        #endif // LAK_USE_SDL
        #endif // LAK_USE_MULTITHREAD
    }

    void run_update(loopData_t *ld)
    {
        assert(ld);
        while (ld->running)
            update_loop(*ld);
    }

    void draw_loop(loopData_t &ld)
    {
        #ifdef LAK_USE_SDL
        uint64_t &LAST = ld.updateTime;
        uint64_t NOW = SDL_GetPerformanceCounter();
        bool vsync = SDL_GL_GetSwapInterval() != 0;

        #ifdef LAK_USE_MULTITHREAD
        double deltaError = ld.drawDelta - ld.targetDrawTime;
        deltaError = deltaError > 0.0 ? deltaError - (ld.targetDrawTime * std::floor(deltaError / ld.targetDrawTime)) : 0.0;
        ld.drawDelta = deltaError + ((NOW - LAST) / (double) SDL_GetPerformanceFrequency());
        #else
        ld.drawDelta = (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
        #endif // LAK_USE_MULTITHREAD

        #ifdef LAK_USE_MULTITHREAD
        while(!vsync && ld.drawDelta < ld.targetDrawTime)
        {
            LAST = NOW;
            NOW = SDL_GetPerformanceCounter();
            ld.drawDelta += (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
            std::this_thread::yield();
        }
        #endif // LAK_USE_MULTITHREAD
        #endif // LAK_USE_SDL

        #ifdef LAK_USE_MULTITHREAD
        ticket_t windowt = ld.windowq.lock();
        #endif // LAK_USE_MULTITHREAD

        #ifdef LAK_USE_SDL
        SDL_GL_MakeCurrent(ld.window, ld.context);
        threadData.haveContext = true;
        #endif // LAK_USE_SDL

        #ifdef LAK_USE_MULTITHREAD
        ticket_t drawt = ld.drawq.lock();
        #endif // LAK_USE_MULTITHREAD

        draw(ld);

        #ifdef LAK_USE_MULTITHREAD
        drawt = nullptr;
        #endif // LAK_USE_MULTITHREAD

        #ifdef LAK_USE_SDL
        SDL_GL_SwapWindow(ld.window);
        SDL_GL_MakeCurrent(ld.window, 0);
        threadData.haveContext = false;
        #endif // LAK_USE_SDL

        #ifdef LAK_USE_MULTITHREAD
        windowt = nullptr;
        #endif // LAK_USE_MULTITHREAD

        #ifdef LAK_USE_SDL
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        ld.drawDelta += (NOW - LAST) / (double) SDL_GetPerformanceFrequency();
        #endif // LAK_USE_SDL
    }

    void run_draw(loopData_t *ld)
    {
        assert(ld);
        while (ld->running)
            draw_loop(*ld);
    }
}

int main()
{
    using namespace lak;
    loopData_t ld;
    threadData.haveContext = true;
    ld.running = true;
    init(ld);
    #ifdef LAK_USE_SDL
    ld.events.reserve(100);
    #endif // LAK_USE_SDL

    #ifdef LAK_USE_MULTITHREAD
    #ifdef LAK_USE_SDL
    thread update(run_update, &ld);
    #endif // LAK_USE_SDL
    thread draw(run_draw, &ld);
    #endif // LAK_USE_MULTITHREAD

    while(ld.running)
    {
        #ifndef LAK_USE_MULTITHREAD
        #ifdef LAK_USE_SDL

        event(ld);

        #endif // LAK_USE_SDL

        draw_loop(ld);
        update_loop(ld);

        #else
        #ifdef LAK_USE_SDL

        ticket_t windowt = ld.windowq.lock();
        ticket_t eventt = ld.eventq.lock();

        SDL_GL_MakeCurrent(ld.window, ld.context);
        threadData.haveContext = true;
        event(ld);
        SDL_GL_MakeCurrent(ld.window, 0);
        threadData.haveContext = false;

        eventt = nullptr;
        windowt = nullptr;

        #else

        update_loop(ld);

        #endif // LAK_USE_SDL
        #endif // LAK_USE_MULTITHREAD
    }

    #ifdef LAK_USE_MULTITHREAD
    #ifdef LAK_USE_SDL
    update.join();
    #endif // LAK_USE_SDL
    draw.join();
    #endif // LAK_USE_MULTITHREAD

    #ifdef LAK_USE_SDL
    SDL_GL_MakeCurrent(ld.window, ld.context);
    threadData.haveContext = true;
    #endif // LAK_USE_SDL

    shutdown(ld);
    return EXIT_SUCCESS;
}