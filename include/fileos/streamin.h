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
#ifndef fileos_streamin_h
#define fileos_streamin_h

#include "fileos/config.h"
#include "containos/ref.h"

namespace fileos {

class StreamIn
{
public:
    enum SeekFrom {
        seek_from_start = 0,
        seek_from_current,
        seek_from_end
    };

    virtual ~StreamIn() {}

    virtual uint32_t read(void* destBuffer, uint32_t size) = 0;
    virtual uint64_t seek(SeekFrom from, int64_t offset) = 0;

    virtual uint64_t position() const = 0;
    virtual uint64_t size() const = 0;
    virtual bool isEos() const = 0;
    virtual bool canSeek() const = 0;
    virtual bool isInMemory() const = 0;

    // read shorthands (default implementation uses read())
    virtual uint32_t readLine(char* buffer, uint32_t bufferSize);
    //virtual void readSizeString(k::String& result);

    template<typename T> T read();
    template<typename T> void read(T& result);

protected:
    StreamIn() : REF_STORAGE_INIT() {}

    REF_STORAGE(StreamIn,uint32_t);
};

} // end of fileos

#include "fileos/streamin.inl"

#endif
