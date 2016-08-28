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
#if defined(FILEOS_MACOSX)
#include "fileos/filein.h"
#include <stdio.h>

namespace fileos {

FileIn::~FileIn()
{
    ::fclose((FILE*)m_handle);
}

FileIn::FileIn(void* handle)
    : StreamIn()
    , m_handle(handle)
    , m_position(0)
    , m_size(0)
{
    ::fseek((FILE*)m_handle, 0, SEEK_END);
    m_size = ::ftell((FILE*)m_handle);
    ::fseek((FILE*)m_handle, 0, SEEK_SET);
}

uint32_t FileIn::read(void* destBuffer, uint32_t size)
{
    uint32_t readSize = ::fread(destBuffer, 1, size, (FILE*)m_handle);
    m_position += readSize;
    return readSize;
}

uint64_t FileIn::seek(SeekFrom from, int64_t offset)
{
    switch(from) {
        case seek_from_start:   ::fseek((FILE*)m_handle, offset, SEEK_SET);    break;
        case seek_from_current: ::fseek((FILE*)m_handle, offset, SEEK_CUR);    break;
        case seek_from_end:     ::fseek((FILE*)m_handle, offset, SEEK_END);    break;
    }
    m_position = ftell((FILE*)m_handle);
    return m_position;
}

uint64_t FileIn::position() const
{
    return m_position;
}

uint64_t FileIn::size() const
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

bool FileIn::isInMemory() const
{
    return false;
}

FileIn* FileIn::open(char const* filename)
{
    FILE* handle = fopen(filename, "rb");
    if(handle == 0) {
        return nullptr;
    }
    return new FileIn(handle);
}

FileIn* FileIn::open(wchar_t const* filename)
{
    return open((char const*)filename);
}

FileIn* FileIn::open(Path const& filename)
{
    wchar_t buffer[1024];
    filename.convertTo(buffer, 1024);
    return open(buffer);
}

} // end of namespace
#endif
