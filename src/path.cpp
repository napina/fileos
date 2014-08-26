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
//#include <locale>

namespace fileos {

void trimEndSlashes(uint16_t const*& str, size_t& length)
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

Path::Path(containos::Utf8 const& buffer)
    : m_buffer(buffer)
{
}

Path::~Path()
{
}

void Path::reserve(size_t capasity)
{
    m_buffer.reserve(capasity);
}

Path Path::parent() const
{
    containos::Utf8::const_iterator it = m_buffer.findLast('/');
    if(it != m_buffer.end()) {
        return Path(m_buffer.slice(it));
    }
    return Path("");
}

containos::Utf8Slice Path::drive() const
{
    containos::Utf8::const_iterator it = m_buffer.findFirst(':');
    if(it != m_buffer.end()) {
        return m_buffer.slice(m_buffer.begin(), it);
    }
    return m_buffer.slice(m_buffer.begin(), m_buffer.begin());
}

containos::Utf8Slice Path::filename() const
{
    containos::Utf8::const_iterator it = m_buffer.findLast('/');
    if(it != m_buffer.end()) {
        ++it;
        return m_buffer.slice(it, m_buffer.end());
    }
    it = m_buffer.findFirst(':');
    if(it != m_buffer.end()) {
        ++it;
        return m_buffer.slice(it, m_buffer.end());
    }
    return m_buffer.slice();
}

containos::Utf8Slice Path::extension() const
{
    containos::Utf8::const_iterator it = m_buffer.findLast('/');
    if(it != m_buffer.end()) {
        it = m_buffer.findLast(it, '.');
        if(it != m_buffer.end()) {
            ++it;
            return m_buffer.slice(it, m_buffer.end());
        }
    }
    return m_buffer.slice(it, it);
}

Path Path::relativeTo(Path const& base) const
{
    base;
    fileos_todo("Implement Path relativeTo");
    return *this;
}

void Path::fixSlashes()
{
    // change slashes
    m_buffer.replace('\\', '/');

    Utf8Slice slice = m_buffer.slice();

    // trim begin slashes
    while(*slice.m_begin == '/')
        ++slice.m_begin;
    
    // trim end slashes
    while(slice.m_end[-1] == '/')
        --slice.m_end;

    m_buffer.trim(slice);
}

void Path::trimFolders()
{
    uint8_t const* folders[32];
    uint32_t folderDots = 0;
    uint32_t folderDotDots = 0;
    size_t folderCount = 0;

    // Two passes. In first pass gather folder offsets.
    uint8_t const* folderPtr = m_buffer.data();
    uint8_t const* ptr = m_buffer.data();
    uint8_t const* end = ptr + m_buffer.dataCount();
    while(ptr != end) {
        if(*ptr++ != '/')
            continue;

        folders[folderCount] = folderPtr;
        if(folderPtr[0] == '.') {
            if(folderPtr[1] == '.' && folderPtr[2] == '/') {
                folderDotDots |= 1 << folderCount;
            } else if(folderPtr[1] == '/') {
                folderDots |= 1 << folderCount;
            }
        }
        ++folderCount;
        folderPtr = ptr;
    }
    folders[folderCount] = folderPtr;

    // no need to do anything if less than 2 folders
    if(folderCount < 2)
        return;

    // Then remove folder that are ..
    for(size_t i = 1; i < folderCount; ++i) {
        const uint32_t mask = 1 << i;
        if((folderDotDots & mask) != 0) {
            // TODO
        }
    }
}

} // end of fileos
