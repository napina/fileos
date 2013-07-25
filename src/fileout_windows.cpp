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
#include <windows.h>

namespace fileos {

FileOut::~FileOut()
{
    ::FlushFileBuffers(m_handle);
    ::CloseHandle(m_handle);
}

FileOut::FileOut(void* handle)
    : StreamOut()
    , m_handle(handle)
    , m_position(0)
    , m_size(0)
{
    LONG offsetHigh;
    DWORD offsetLow = ::SetFilePointer(m_handle, 0, &offsetHigh, FILE_CURRENT);
    m_position = offsetLow | (int64_t(offsetHigh) << 32);
    m_size = m_position;
}

uint32_t FileOut::write(void const* srcBuffer, uint32_t size)
{
    DWORD writeSize = 0;
    ::WriteFile(m_handle, srcBuffer, (DWORD)size, &writeSize, NULL);
    m_position += writeSize;
    m_size += writeSize;
    return writeSize;
}

void FileOut::flush()
{
    ::FlushFileBuffers(m_handle);
}

uint64_t FileOut::position() const
{
    return m_position;
}

uint64_t FileOut::size() const
{
    return m_size;
}

FileOut* FileOut::open(wchar_t const* filename, bool append)
{
    DWORD dwDesiredAccess = FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | (append ? FILE_APPEND_DATA : 0);
    DWORD dwShareMode = FILE_SHARE_READ;
    DWORD dwCreationDisposition = CREATE_ALWAYS;
    DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    HANDLE handle = ::CreateFileW(filename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL);
    if(handle == INVALID_HANDLE_VALUE) {
        DWORD error = ::GetLastError();
        return nullptr;
    }
    return new FileOut(handle);
}

} // end of namespace
