/*=============================================================================

Copyright (c) 2013 Ville Ruusutie

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

=============================================================================*/
#pragma once
#ifndef fileos_common_h
#define fileos_common_h

#if defined(__linux__) && defined(__ELF__)
#   define FILEOS_LINUX
#   define FILEOS_ARCH32
#elif defined(__APPLE__) && defined(__MACH__)
#   define FILEOS_MACOSX
#   define FILEOS_ARCH32
#elif defined(_WIN64) || defined(_M_X64)
#   define FILEOS_WINDOWS
#   define FILEOS_ARCH64
#elif defined(_WIN32) || defined(_M_IX86)
#   define FILEOS_WINDOWS
#   define FILEOS_ARCH32
#endif

#define fileos_tostring_impl(x)     #x
#define fileos_tostring(x)          fileos_tostring_impl(x)

#if defined(_MSC_VER)
#   define fileos_lineinfo          __FILE__ "(" fileos_tostring(__LINE__) ")"
#   define fileos_todo(msg)         __pragma(message(fileos_lineinfo ": TODO " msg))
#elif defined(__GNUC__)
#   define fileos_lineinfo          __FILE__ ":" fileos_tostring(__LINE__)
#   define fileos_todo(msg)         __Pragma(message("TODO " msg))
#   define __forceinline            inline __attribute__((always_inline))
#   define __restrict               __restrict__
#else
#   define fileos_lineinfo          __FILE__ ":" fileos_tostring(__LINE__)
#   define fileos_todo(msg)
#   define __forceinline            inline
#   define __restrict
#endif

#ifndef fileos_assert
#define fileos_assert(Test)
#endif

#ifndef fileos_strlen
#define fileos_strlen(Str)          ::_mbstrlen_l(Str, nullptr)
#endif
//----------------------------------------------------------------------------

namespace fileos {

typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
typedef char                utf8_t;

} // end of fileos

#endif
