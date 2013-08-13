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
#ifndef fileos_streamout_h
#define fileos_streamout_h

#include "fileos/config.h"
#include "containos/ref.h"

namespace fileos {

class StreamOut
{
public:
    virtual ~StreamOut() {}

    virtual uint32_t write(void const* srcBuffer, uint32_t size) = 0;
    virtual void flush() = 0;

    virtual uint64_t position() const = 0;
    virtual uint64_t size() const = 0;

    // write shorthands (default implementation uses write())
    virtual void writeSizeString(char const* text, uint32_t size);

    template<typename T> void write(T const& data);

protected:
    REF_STORAGE(StreamOut,uint32_t);
};

} // end of fileos

#include "fileos/streamout.inl"

#endif
