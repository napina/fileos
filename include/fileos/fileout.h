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
#pragma once
#ifndef fileos_fileout_h
#define fileos_fileout_h

#include "fileos/streamout.h"
#include "fileos/fileinfo.h"
#include "fileos/path.h"

namespace fileos {

class FileOut : public StreamOut
{
public:
    virtual ~FileOut();

    virtual uint32_t write(void const* srcBuffer, uint32_t size);
    virtual void flush();
    virtual bool setWriteTime(const FileTime& filetime);

    virtual uint64_t position() const;
    virtual uint64_t size() const;

    static FileOut* open(char const* filename, bool append);
    static FileOut* open(wchar_t const* filename, bool append);
    static FileOut* open(Path const& filename, bool append);

private:
    FileOut(void* handle);

private:
    void* m_handle;
    uint64_t m_position;
    uint64_t m_size;
};

} // end of fileos

#endif
