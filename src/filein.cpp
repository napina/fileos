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
#include "fileos/filein.h"
#include <stdio.h>

namespace fileos {

FileIn::~FileIn()
{
    fileos_assert(m_handle != nullptr);
    ::fflush(reinterpret_cast<FILE*>(m_handle));
    ::fclose(reinterpret_cast<FILE*>(m_handle));
}

FileIn::FileIn(void* handle)
    : StreamIn()
    , m_handle(handle)
    , m_position(0)
    , m_size(0)
{
    fileos_assert(m_handle != nullptr);

    ::fseek(reinterpret_cast<FILE*>(m_handle), 0, SEEK_END);
    m_size = (int32_t)::ftell(reinterpret_cast<FILE*>(m_handle));
    ::fseek(reinterpret_cast<FILE*>(m_handle), 0, SEEK_SET);
}

size_t FileIn::read(void* destBuffer, size_t size)
{
    size_t readSize = ::fread(destBuffer, 1, size, reinterpret_cast<FILE*>(m_handle));
    m_position += readSize;
    return readSize;
}

size_t FileIn::seek(SeekFrom from, int count)
{
    switch(from) {
        case seek_from_start:   ::fseek(reinterpret_cast<FILE*>(m_handle), count, SEEK_SET);    break;
        case seek_from_current: ::fseek(reinterpret_cast<FILE*>(m_handle), count, SEEK_CUR);    break;
        case seek_from_end:     ::fseek(reinterpret_cast<FILE*>(m_handle), count, SEEK_END);    break;
    }
    m_position = ::ftell(reinterpret_cast<FILE*>(m_handle));
    return m_position;
}

size_t FileIn::position() const
{
    return m_position;
}

size_t FileIn::size() const
{
    return m_size;
}

bool FileIn::isEos() const
{
    return m_position >= m_size;
}

bool FileIn::canSeek() const
{
    return true;
}

FileIn* FileIn::open(char const* filename)
{
    FILE* stdfile = ::fopen(filename, "rb");
    if(stdfile != nullptr) {
        return new FileIn(stdfile);
    }
    return nullptr;
}

} // end of namespace
