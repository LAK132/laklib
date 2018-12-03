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

#include <vector>
#include <string>
#include <assert.h>
#include <atomic>
#include <thread>
#include <memory>

#include "types/queue.h"

#ifndef LAK_MAINLOOP_H
#define LAK_MAINLOOP_H

struct userData_t; // must be defined in user code

void update(std::atomic_bool *const running, userData_t *const data);
void draw(std::atomic_bool *const running, userData_t *const data);

namespace lak
{
    using std::vector;
    using std::atomic;
    using std::atomic_bool;
    using std::thread;
    using std::shared_ptr;
    using std::make_shared;

    extern queue_t update_thread_lock;
    static void update_loop(atomic_bool *const running, userData_t *const data)
    {
        assert(running && data);
        while (running && *running)
        {
            assert(data);
            withQueue(update_thread_lock, tl)
            {
                update(running, data);
            }
            std::this_thread::yield();
        }
    }

    static shared_ptr<thread> update_thread(atomic_bool *const running, userData_t *const data)
    {
        return make_shared<thread>(update_loop, running, data);
    }

    static void draw_loop(atomic_bool *const running, userData_t *const data)
    {
        assert(running && data);
        while (running && *running)
        {
            assert(data);
            withQueue(update_thread_lock, tl)
            {
                draw(running, data);
            }
            std::this_thread::yield();
        }
    }

    static shared_ptr<thread> draw_thread(atomic_bool *const running, userData_t *const data)
    {
        return make_shared<thread>(draw_loop, running, data);
    }
}

#ifdef LAK_MAINLOOP_IMPLEM
#   ifndef LAK_MAINLOOP_HAS_IMPLEM
#       define LAK_MAINLOOP_HAS_IMPLEM
#       include "runtime/mainloop.cpp"
#   endif // LAK_MAINLOOP_HAS_IMPLEM
#endif // LAK_MAINLOOP_IMPLEM

#endif // LAK_MAINLOOP_H