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
//#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include "fileos/path.h"
#include <wchar.h>
//#include <string.h>

namespace fileos {

Path::Path()
    : m_buffer(nullptr)
{
}

Path::Path(utf8_t const* path)
    : m_buffer(nullptr)
{
    construct(path, fileos_strlen(path));
}

Path::Path(utf8_t const* path, size_t length)
    : m_buffer(nullptr)
{
    construct(path, length);
}

Path::Path(Path const& other)
    : m_buffer(other.m_buffer)
{
    if(m_buffer)
        ++m_buffer->m_refCount;
}

Path::Path(wchar_t const* path)
    : m_buffer(nullptr)
{
    construct(path, ::wcslen(path));
}

Path::Path(wchar_t const* path, size_t length)
    : m_buffer(nullptr)
{
    construct(path, length);
}

Path::~Path()
{
    destruct();
}

void Path::construct(utf8_t const* str, size_t length)
{
    fileos_assert(m_buffer == nullptr);
    m_buffer = (Buffer*)::malloc(length + sizeof(Buffer));
    ::memcpy(m_buffer->m_data, str, length);
    m_buffer->m_data[length] = 0;
    m_buffer->m_length = uint32_t(length);
    m_buffer->m_refCount = 1;
}

void Path::construct(utf8_t const* a, size_t aLength, utf8_t const* b, size_t bLength)
{
    fileos_assert(m_buffer == nullptr);
    uint32_t newLength = uint32_t(aLength + 1 + bLength);
    m_buffer = (Buffer*)::malloc(newLength + sizeof(Buffer));
    ::memcpy(m_buffer->m_data, a, aLength);
    m_buffer->m_data[aLength] = '/';
    ::memcpy(m_buffer->m_data + aLength + 1, b, bLength);
    m_buffer->m_data[newLength] = 0;
    m_buffer->m_length = newLength;
    m_buffer->m_refCount = 1;
}

void Path::construct(wchar_t const* str, size_t length)
{
    fileos_assert(m_buffer == nullptr);
    size_t bufferSize = ::wcsrtombs(nullptr, &str, length, nullptr);
    m_buffer = (Buffer*)::malloc(bufferSize + sizeof(Buffer));
    ::wcsrtombs(m_buffer->m_data, &str, length, nullptr);
    m_buffer->m_data[bufferSize] = 0;
    m_buffer->m_length = uint32_t(length);
    m_buffer->m_refCount = 1;
}

void Path::destruct()
{
    if(m_buffer == nullptr)
        return;

    --m_buffer->m_refCount;
    if(m_buffer->m_refCount != 0)
        return;

    ::free(m_buffer);
    m_buffer = nullptr;
}

} // end of fileos
