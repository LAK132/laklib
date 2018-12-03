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

#include <memory>
#include <mutex>

#ifndef LAK_QUEUE_NO_DEFINES
#define withQueue(queue, ticket) if (lak::ticket_t ticket = queue.lock(); ticket)
#endif // LAK_QUEUE_NO_DEFINES

#ifndef LAK_QUEUE_H
#define LAK_QUEUE_H

namespace lak
{
    using std::mutex;
    using std::lock_guard;
    using std::shared_ptr;
    using std::weak_ptr;
    using std::make_shared;
    using std::unique_ptr;
    using std::make_unique;

    struct queue_t;

    struct _ticket
    {
    private:
        shared_ptr<mutex> mtx = make_shared<mutex>();
        unique_ptr<lock_guard<mutex>> guard;
    public:
        _ticket();
        friend queue_t;
    };

    using ticket_t = shared_ptr<_ticket>;
    using weak_ticket_t = weak_ptr<_ticket>;

    struct queue_t
    {
    private:
        weak_ptr<_ticket> tail;
        mutex _lock;
    public:
        ticket_t lock();
    };
}

#ifdef LAK_QUEUE_IMPLEM
#   ifndef LAK_QUEUE_HAS_IMPLEM
#       define LAK_QUEUE_HAS_IMPLEM
#       include "types/queue.cpp"
#   endif // LAK_QUEUE_HAS_IMPLEM
#endif // LAK_QUEUE_IMPLEM

#endif // LAK_QUEUE_H