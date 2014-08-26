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

namespace fileos {

__forceinline Path::Path()
    : m_buffer()
{
}

__forceinline Path::Path(Path const& other)
    : m_buffer(other.m_buffer)
{
}

__forceinline Path::Path(char const* path)
    : m_buffer(path)
{
    fixSlashes();
}

__forceinline Path::Path(char const* path, size_t length)
    : m_buffer(path, length)
{
    fixSlashes();
}

__forceinline Path::Path(wchar_t const* path)
    : m_buffer(path)
{
    fixSlashes();
}

__forceinline Path::Path(wchar_t const* path, size_t length)
    : m_buffer(path, length)
{
    fixSlashes();
}

template<size_t Count>
__forceinline Path::Path(char const (&str)[Count])
    : m_buffer(str)
{
    fixSlashes();
}

template<size_t Count>
__forceinline Path::Path(wchar_t const (&str)[Count])
    : m_buffer(str)
{
    fixSlashes();
}

__forceinline void Path::set(Path const& other)
{
    m_buffer = other.m_buffer;
}

template<typename T>
__forceinline void Path::set(T const* path)
{
    m_buffer.set(path);
    fixSlashes();
}

template<typename T>
__forceinline void Path::set(T const* path, size_t count)
{
    m_buffer.set(path, count);
    fixSlashes();
}

template<typename T>
__forceinline void Path::append(T const* str)
{
    m_buffer.append(str);
    fixSlashes();
}

template<typename T>
__forceinline void Path::append(T const* str, size_t count)
{
    m_buffer.append(str, count);
    fixSlashes();
}

__forceinline uint8_t const* Path::data() const
{
    return m_buffer.data();
}

__forceinline size_t Path::length() const
{
    return m_buffer.length();
}

__forceinline bool Path::operator==(Path const& other) const
{
    return m_buffer == other.m_buffer;
}

__forceinline bool Path::operator==(char const* str) const
{
    return m_buffer == str;
}

__forceinline bool Path::operator==(wchar_t const* str) const
{
    return m_buffer == str;
}

} // end of fileos

#endif
