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
#include <windows.h>

namespace fileos {

FileIn::~FileIn()
{
    ::CloseHandle(m_handle);
}

FileIn::FileIn(void* handle)
    : StreamIn()
    , m_handle(handle)
    , m_position(0)
    , m_size(0)
{
    FILE_STANDARD_INFO info;
    ::GetFileInformationByHandleEx(m_handle, FileStandardInfo, &info, sizeof(FILE_STANDARD_INFO));
    m_size = info.EndOfFile.QuadPart;
}

uint32_t FileIn::read(void* destBuffer, uint32_t size)
{
    DWORD readSize = 0;
    ::ReadFile(m_handle, destBuffer, (DWORD)size, &readSize, NULL);
    m_position += readSize;
    return readSize;
}

uint64_t FileIn::seek(SeekFrom from, int64_t offset)
{
    LARGE_INTEGER loffset;
    loffset.QuadPart = offset;
    LARGE_INTEGER lposition;
    switch(from) {
        case seek_from_start:   ::SetFilePointerEx(m_handle, loffset, &lposition, FILE_BEGIN);      break;
        case seek_from_current: ::SetFilePointerEx(m_handle, loffset, &lposition, FILE_CURRENT);    break;
        case seek_from_end:     ::SetFilePointerEx(m_handle, loffset, &lposition, FILE_END);        break;
    }
    m_position = lposition.QuadPart;
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

FileIn* FileIn::open(utf8_t const* filename)
{
    DWORD dwDesiredAccess = FILE_READ_DATA | FILE_READ_ATTRIBUTES;
    DWORD dwShareMode = FILE_SHARE_READ;
    DWORD dwCreationDisposition = OPEN_EXISTING;
    DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_READONLY;// | FILE_FLAG_NO_BUFFERING;
    HANDLE handle = ::CreateFileA(filename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL);
    if(handle == INVALID_HANDLE_VALUE) {
        DWORD error = ::GetLastError();
        return nullptr;
    }
    return new FileIn(handle);
}

} // end of namespace
