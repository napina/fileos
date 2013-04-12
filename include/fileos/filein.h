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
#ifndef fileos_filein_h
#define fileos_filein_h

#include "fileos/streamin.h"

namespace fileos {

class FileIn : public StreamIn
{
public:
    virtual ~FileIn();

    virtual size_t read(void* destBuffer, size_t size);
    virtual size_t seek(SeekFrom from, int offset);

    virtual size_t position() const;
    virtual size_t size() const;
    virtual bool isEos() const;
    virtual bool canSeek() const;

    static FileIn* open(char const* filename);

private:
    FileIn(void* handle);

private:
    void* m_handle;
    size_t m_position;
    size_t m_size;
};

} // end of fileos

#endif