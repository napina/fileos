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
#include "fileos/fileout.h"
#include "fileos/fileinfo.h"
#include <stdio.h>

namespace fileos {

FileOut::~FileOut()
{
    ::fclose((FILE*)m_handle);
}

FileOut::FileOut(void* handle)
    : StreamOut()
    , m_handle(handle)
    , m_position(0)
    , m_size(0)
{
    m_position = ::ftell((FILE*)m_handle);
    m_size = m_position;
}

uint32_t FileOut::write(void const* srcBuffer, uint32_t size)
{
    uint32_t writeSize = ::fwrite(srcBuffer, 1, size, (FILE*)m_handle);
    m_position += writeSize;
    m_size += writeSize;
    return writeSize;
}

void FileOut::flush()
{
    ::fflush((FILE*)m_handle);
}

uint64_t FileOut::position() const
{
    return m_position;
}

uint64_t FileOut::size() const
{
    return m_size;
}

FileOut* FileOut::open(char const* filename, bool append)
{
    FILE* handle = ::fopen(filename, append ? "a" : "w");
    if(handle == 0) {
        //DWORD error = ::GetLastError();
        return nullptr;
    }
    return new FileOut(handle);
}

FileOut* FileOut::open(wchar_t const* filename, bool append)
{
    return open((char const*)filename, append);
}

FileOut* FileOut::open(Path const& filename, bool append)
{
    wchar_t buffer[1024];
    filename.convertTo(buffer, 1024);
    return open(buffer, append);
}

bool FileOut::setWriteTime(const FileTime& time)
{
    // TODO
    return false;
}

} // end of namespace
#endif
