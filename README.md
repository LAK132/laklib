# About

I started working on this collection of libraries in my computer graphics class at University, so that I didn't go insane every time I needed to do something as simple as open a window! As of writing, C++ has no standard support for graphics, so I wrote my own wrappers for OpenGL and SDL that fit my programming style. Since then, I have created a few other libraries that I use regularly in C++, so I decided to put them all into one place that I can easily merge into new projects!

At the time of writing, the documentation for these libraries is terrible. If anyone else find interest in these libraries, I may consider going through and documenting everything better.

When I started writing these libraries I opted to wrap everything in a namespace, and because I have no imagination I chose to use my initials (`lak`).

All of my libraries should have a license in the source, but they are otherwise MIT licensed. See the end of this document for the license of the other libraries included here.

# Style

Generally speaking, I use `cameCase_t` for type names (with the exception of `stride_vector` which I made to mimic `std::vector`), `camelCase` for members, `snake_case` for things that shouldn't be used often and `_thisone` for things that you really should not touch (catastrophic threading errors in the case of `queue_t`).

For the most part, the libraries I've written are designed to be used in an OOP way, however I think they could very easily be reused in a DOP way (ie just use them for storage and don't use their member functions). To make it easier to use them in a DOP way, all members that won't cause a catastrophic failure are public (even when they are intended to be readonly).

# Dependencies

Some of these libraries have dependencies, so to make life easier I have either copied them verbatim (`gl3w`, `glcorearb`, `SDL`) or forked them and included them as submodules (`glm`, `stb`). If you use these submodules then no external dependecies are needed.

# What's included:

# types/

## colour

This library implements `color_t<GLenum glformat, GLenum gltype>` and several other variations that cover most if not all of the different OpenGL colour options ie `colorRGB8_t`, `colorRGBA16_t`, etc. (see end of `color.h` for the full list).

## image

This library implements `image_t<GLenum glformat, GLenum gltype>`, which is effectively a 2D vector of `color_t`.

## texture

This library implements `texparam_t` and `texture_t` to make loading textures with OpenGL less of a pain in the arse.

## queue

This library implements `queue_t` and `ticket_t` (aka `shared_ptr<_ticket>`). I primarily use these two to manage threading in `runtime/mainloop`. I'm not sure how reliable these actually are, but the reality is they're REALLY small and I've never had any issue with them.

`queue_t` is basically just a wrapper for `mutex`. calling `.lock()` on an instance of `queue_t` will block until that call bubbles to the top of the queue, at which point it returns a `ticket_t`. `queue_t` keeps a weak reference to the last returned ticket (tail), if the weak reference is invalid, `.lock()` immediately unblocks. If the weak reference is valid, `.lock()` creates a new ticket that references the current tail ticket, then points the tail at the newly created ticket. `.lock()` will then block until the new tickets weak ref becomes invalid (meaning it is at the front of the queue)

## shader

Implements `shaderAttribute_t`, `shaderProgram_t` and typedefs `shared_ptr<shaderProgram_t` as `shader_t` to manage OpenGL shaders/programs.

## mesh

Implements `vertexElement_t`, `vertexBuffer_t`, `vertexArray_t` and `mesh_t` to manage OpenGL buffers (mainly for 3D models)

## json

This library is a mess, but I couldn't find any other JSON library that I liked... so I wrote my own.

## stride_vector

This library was created to hopefully improve shader performance by interlacing the buffer data before sending it to the GPU. Basically it allows `mesh_t` to put all the elements for a single vertex in a contiguous block of memory, rather than each element being in a contiguous block in the buffer. Theoretically this should redue GPU chache misses.

## type_list

Similar to `std::variant`, but can do compile-time iteration and stuff (WIP)

# utils/

## crc32_hash

Compile-time crc32 hashing!

## ldebug

Some macros for debugging

## obj

This library parses `.obj` model files. At the time of writing, it is set up to convert quads/n-gons into tris (the only thing my engine supports atm)

## pnm

This library parses `.pnm` image files

## stream

Various functions and macros to make life a little easier when you need to crawl through `strings` and or `streams`

## type

Some C++ type/template utilities I've written while working on other libraries

# runtime/

## mainloop

This library effectively replaces `int main()` with `void init(loopData)`, `void update(loopData)`, `void draw(loopData)` and `void shutdown(loopData)`. If used with `SDL` and multithreading, it will manage the OpenGL context and run `update` in a seperate thread, allowing you to make *many* update calls per `draw` call. I found myself rewriting this every time I wanted to make a multithreaded program, so I generalised it and put it here!

# cql/

This is a quaternion and dual quaternion library I wrote to avoid having to use matrices in my graphics engine. I suggest reading [this paper](https://cs.gmu.edu/~jmlien/teaching/cs451/uploads/Main/dual-quaternion.pdf) ([Internet Archive mirror](https://web.archive.org/web/20180622093933/https://cs.gmu.edu/~jmlien/teaching/cs451/uploads/Main/dual-quaternion.pdf)) if you'd like to know more about why dual quaternions are so cool

# Libraries I didn't make that I've included here

## SDL/

This is the `SDL` headers and DLLS (x86 and x64) for Windows. If you're using Linux you will need to use your system's headers and libs instead.

SDL 2.0+ uses the zlib license (https://www.libsdl.org/license.php).

## GL/

This includes `gl3w` (https://github.com/skaslev/gl3w) and `glcorearb.h` (https://www.khronos.org/registry/OpenGL/api/GL/). Both of these are created by `gl3w`'s `gl3w_gen.py`, however I have included them as-is so that these libraries aren't dependent on python.

`gl3w` is public domain/UNLICENSE (https://github.com/skaslev/gl3w/blob/master/UNLICENSE).

## glm/

This is a fork of `glm` by `g-truc` (https://github.com/g-truc/glm). 3D maths would be hell without this!

`glm` uses the MIT license (https://github.com/g-truc/glm/blob/master/manual.md#section0).

## stb/

This is a fork of `stb` by `nothings` (https://github.com/nothings/stb). These libraries make C++ just that little bit more bearable.

`stb` is dual licensed public domain (https://github.com/nothings/stb/blob/master/docs/why_public_domain.md). See the headers for their licenses