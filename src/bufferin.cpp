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
#include "fileos/bufferin.h"
#include <stdio.h>

namespace fileos {

BufferIn::~BufferIn()
{
}

BufferIn::BufferIn(char* buffer, size_t size)
    : StreamIn()
    , m_buffer(buffer)
    , m_position(0)
    , m_size(size)
{
}

size_t BufferIn::read(void* destBuffer, size_t size)
{
    fileos_todo("add read check");
    ::memcpy(destBuffer, m_buffer, size);
    return size;
}

size_t BufferIn::seek(SeekFrom from, int count)
{
    switch(from) {
        case seek_from_start:   m_position = (size_t)count;             break;
        case seek_from_current: m_position += (size_t)count;            break;
        case seek_from_end:     m_position = (size_t)(m_size - count);  break;
    }
    return m_position;
}

size_t BufferIn::position() const
{
    return m_position;
}

size_t BufferIn::size() const
{
    return m_size;
}

bool BufferIn::isEos() const
{
    return m_position >= m_size;
}

bool BufferIn::canSeek() const
{
    return true;
}

} // end of namespace
