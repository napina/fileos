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
#include "pch.h"
#include "fileos/path.h"

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
    if(it == m_buffer.end())
        it = m_buffer.begin();
    it = m_buffer.findLast(it, '.');
    if(it != m_buffer.end()) {
        ++it;
        return m_buffer.slice(it, m_buffer.end());
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
    size_t folderCount = 0;

    // Two passes. In first pass gather folders and get rid of .
    uint8_t const* ptr = m_buffer.data();
    uint8_t const* end = ptr + m_buffer.dataCount();
    folders[folderCount++] = ptr;
    while(ptr != end) {
        if(*ptr++ != '/')
            continue;

        if((ptr[0] != '.') || (ptr[1] != '/')) {
            folders[folderCount++] = ptr;
        }
    }

    // Then remove folder that are ..
    for(size_t i = 0; i < folderCount - 1;) {
        uint8_t const* folder = folders[i++];
        uint8_t const* nextFolder = folders[i];
        if(nextFolder[0] != '.' || nextFolder[1] != '.' || nextFolder[2] != '/')
            continue;
        if(folder[0] != '.' || folder[1] != '.' || folder[2] != '/') {
            // next is .. and this is actual folder. move end and go back one
            --i;
            memcpy(&folders[i], &folders[i+2], sizeof(uint8_t const*) * (folderCount-i-2));
            folderCount -= 2;
            if(i > 0)
                --i;
        }
    }

    // combine result
    uint8_t result[1024];
    uint8_t* writePtr = result;
    for(size_t i = 0; i < folderCount; ++i) {
        uint8_t const* folder = folders[i];
        while(*folder != 0) {
            uint8_t ch = *folder++;
            *writePtr++ = ch;
            if(ch == '/')
                break;
        }
    }
    *writePtr = 0;
    m_buffer.set((uint8_t const*)result);
}

} // end of fileos
