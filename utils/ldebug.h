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

#ifndef LASSERT
#include <assert.h> // note: assert already handles NDEBUG
#define LASSERT_MSG(message) #message
#define LASSERT(assertion, message) assert((assertion) && (LASSERT_MSG(__FILE__) "(" LASSERT_MSG(__LINE__) ")" message))
#endif // LASSERT

#ifndef NDEBUG // debug mode
#include <iostream>

#ifndef LDEBUG
#define LDEBUG(x) std::cout << __FILE__ << "(" << std::dec << __LINE__ << ") " << x << std::endl
#endif // LDEBUG

#ifndef LERROR
#define LERROR(x) std::cerr << __FILE__ << "(" << std::dec << __LINE__ << ") " << x << std::endl
#endif // LERROR

#ifndef LERRLOG
#define LERRLOG(x) std::clog << __FILE__ << "(" << std::dec << __LINE__ << ") " << x << std::endl
#endif // LERRLOG

#ifndef LWDEBUG
#define LWDEBUG(x) std::wcout << __FILE__ << "(" << std::dec << __LINE__ << ") " << x << std::endl
#endif // LWDEBUG

#ifndef LWERROR
#define LWERROR(x) std::wcerr << __FILE__ << "(" << std::dec << __LINE__ << ") " << x << std::endl
#endif // LWERROR

#ifndef LWERRLOG
#define LWERRLOG(x) std::wclog << __FILE__ << "(" << std::dec << __LINE__ << ") " << x << std::endl
#endif // LWERRLOG

#else // release mode

#ifndef LDEBUG
#define LDEBUG(x)
#endif // LDEBUG

#ifndef LERROR
#define LERROR(x)
#endif // LERROR

#ifndef LERRLOG
#define LERRLOG(x)
#endif // LERRLOG

#ifndef LWDEBUG
#define LWDEBUG(x)
#endif // LWDEBUG

#ifndef LWERROR
#define LWERROR(x)
#endif // LWERROR

#ifndef LWERRLOG
#define LWERRLOG(x)
#endif // LWERRLOG

#endif // NDEBUG