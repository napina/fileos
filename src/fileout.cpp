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
#include "fileos/fileout.h"
#include <stdio.h>

namespace fileos {

FileOut::~FileOut()
{
    fileos_assert(m_handle != nullptr);
    ::fflush(reinterpret_cast<FILE*>(m_handle));
    ::fclose(reinterpret_cast<FILE*>(m_handle));
}

FileOut::FileOut(void* handle)
    : StreamOut()
    , m_handle(handle)
    , m_position(0)
    , m_size(0)
{
    fileos_assert(m_handle != nullptr);
    m_position = ::ftell(reinterpret_cast<FILE*>(m_handle));
    m_size = m_position;
}

size_t FileOut::write(void const* srcBuffer, size_t size)
{
    size_t writeSize = ::fwrite(srcBuffer, 1, size, reinterpret_cast<FILE*>(m_handle));
    m_position += writeSize;
    m_size += writeSize;
    return writeSize;
}

void FileOut::flush()
{
    ::fflush(reinterpret_cast<FILE*>(m_handle));
}

size_t FileOut::position() const
{
    return m_position;
}

size_t FileOut::size() const
{
    return m_size;
}

FileOut* FileOut::open(char const* filename, bool append)
{
    FILE* stdfile = nullptr;
    if(append)
        ::fopen_s(&stdfile, filename, "ab");
    else
        ::fopen_s(&stdfile, filename, "wb");

    if(stdfile != nullptr) {
        return new FileOut(stdfile);
    }
    return nullptr;
}

} // end of namespace
