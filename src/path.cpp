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
#include <string.h>

namespace fileos {

void trimEndSlashes(utf8_t const*& str, size_t& length)
{
    if(length == 0)
        return;

    // check start
    if((str[0] == '/') | (str[0] == '\\')) {
        --length;
        ++str;
    }
    if(length == 0)
        return;

    // check end
    if((str[length - 1] == '/') | (str[length - 1] == '\\'))
        --length;
}

size_t computeMultiByteLength(wchar_t const* str, size_t charCount)
{
    size_t resultCount = 0;
    char buffer[4];
    mbstate_t state;
    ::mbrlen(nullptr, 0, &state);
    for(size_t i = 0; i < charCount; ++i) {
        size_t count = ::wcrtomb(nullptr, str[i], &state);
        if(count > 4)
            break;
        resultCount += count;
    }
    return resultCount;
}

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
    if(m_buffer) {
        ++(m_buffer->m_refCount);
    }
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

Path Path::parent() const
{
    utf8_t const* str = ::strrchr(m_buffer->m_data, '/');
    if(str == nullptr)
        return Path("");
    Path result;
    result.construct(m_buffer->m_data, ptrdiff_t(str) - ptrdiff_t(m_buffer->m_data));
    return result;
}

utf8_t const* Path::filename() const
{
    utf8_t const* result = ::strrchr(m_buffer->m_data, '/');
    if(result == nullptr)
        return "";
    return ++result;
}

utf8_t const* Path::extension() const
{
    utf8_t const* result = ::strrchr(m_buffer->m_data, '.');
    if(result == nullptr)
        return "";
    if(result[1] == '/')
        return "";
    return ++result;
}

Path Path::relativeTo(Path const& base) const
{
    fileos_todo("Implement Path relativeTo");
    return *this;
}

void Path::construct(utf8_t const* str, size_t length)
{
    fileos_assert(m_buffer == nullptr);
    trimEndSlashes(str, length);

    m_buffer = (Buffer*)::malloc(length + sizeof(Buffer));
    ::memcpy(m_buffer->m_data, str, length);
    m_buffer->m_data[length] = 0;

    m_buffer->m_length = uint32_t(length);
    m_buffer->m_refCount = 1;

    changeSlashes();
    trimFolders();
}

void Path::construct(utf8_t const* a, size_t aLength, utf8_t const* b, size_t bLength)
{
    fileos_assert(m_buffer == nullptr);
    trimEndSlashes(a, aLength);
    trimEndSlashes(b, bLength);

    if(aLength == 0) {
        construct(b, bLength);
    } else if(bLength == 0) {
        construct(a, aLength);
    } else {
        uint32_t newLength = uint32_t(aLength + 1 + bLength);
        m_buffer = (Buffer*)::malloc(newLength + sizeof(Buffer));

        ::memcpy(m_buffer->m_data, a, aLength);
        m_buffer->m_data[aLength] = '/';
        ::memcpy(m_buffer->m_data + aLength + 1, b, bLength);
        m_buffer->m_data[newLength] = 0;

        m_buffer->m_length = uint32_t(newLength);
        m_buffer->m_refCount = 1;

        changeSlashes();
        trimFolders();
    }
}

void Path::construct(wchar_t const* str, size_t length)
{
    fileos_assert(m_buffer == nullptr);

    size_t bufferSize = computeMultiByteLength(str, length);
    m_buffer = (Buffer*)::malloc(bufferSize + sizeof(Buffer));
    ::wcsrtombs(m_buffer->m_data, &str, length, nullptr);
    m_buffer->m_data[bufferSize] = 0;

    m_buffer->m_length = uint32_t(length);
    m_buffer->m_refCount = 1;

    changeSlashes();
    trimFolders();
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

void Path::changeSlashes()
{
    utf8_t* str = ::strchr(m_buffer->m_data, '\\');
    while(str != nullptr) {
        *str = '/';
        str = ::strchr(str, '\\');
    }
}

void Path::trimFolders()
{
    utf8_t* dirStr = ::strstr(m_buffer->m_data, "/../");
    if(dirStr == nullptr)
        return;

    (*dirStr) = 0;
    utf8_t* prevStr = ::strrchr(m_buffer->m_data, '/');
    if(prevStr == nullptr) {
        fileos_todo("fix folder trimming with multiple ..");
        (*dirStr) = '/';
        return;
    }

    ::memcpy(prevStr, dirStr + 3, m_buffer->m_length - ptrdiff_t(dirStr - m_buffer->m_data) + 2);
    m_buffer->m_length = uint32_t(fileos_strlen(m_buffer->m_data));

    trimFolders();
}

} // end of fileos
