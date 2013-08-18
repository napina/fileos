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
#ifndef fileos_path_inl
#define fileos_path_inl

#include <stdlib.h> // TODO get rid of this

namespace fileos {

struct Path::Buffer
{
    uint32_t m_refCount;
    uint32_t m_length;
    utf8_t m_data[1];
};

__forceinline utf8_t const* Path::c_str() const
{
    return m_buffer->m_data;
}

__forceinline size_t Path::length() const
{
    return m_buffer->m_length;
}

__forceinline Path const& Path::catenate(Path const& a, Path const& b)
{
    destruct();
    construct(a.m_buffer->m_data, a.m_buffer->m_length, b.m_buffer->m_data, b.m_buffer->m_length);
    return *this;
}

__forceinline Path const& Path::catenate(Path const& a, utf8_t const* b)
{
    destruct();
    construct(a.m_buffer->m_data, a.m_buffer->m_length, b, fileos_strlen(b));
    return *this;
}

__forceinline Path const& Path::catenate(utf8_t const* a, Path const& b)
{
    destruct();
    construct(a, fileos_strlen(a), b.m_buffer->m_data, b.m_buffer->m_length);
    return *this;
}

__forceinline Path const& Path::catenate(utf8_t const* a, utf8_t const* b)
{
    destruct();
    construct(a, fileos_strlen(a), b, fileos_strlen(b));
    return *this;
}

__forceinline Path catenate(Path const& a, Path const& b)
{
    Path path;
    path.construct(a.m_buffer->m_data, a.m_buffer->m_length, b.m_buffer->m_data, b.m_buffer->m_length);
    return path;
}

__forceinline Path catenate(Path const& a, utf8_t const* b)
{
    Path path;
    path.construct(a.m_buffer->m_data, a.m_buffer->m_length, b, fileos_strlen(b));
    return path;
}

__forceinline Path catenate(utf8_t const* a, Path const& b)
{
    Path path;
    path.construct(a, fileos_strlen(a), b.m_buffer->m_data, b.m_buffer->m_length);
    return path;
}

} // end of fileos

#endif
