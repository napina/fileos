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
#ifndef fileos_streamin_inl
#define fileos_streamin_inl

namespace fileos {

inline StreamIn::StreamIn()
    : REF_STORAGE_INIT()
{
}

inline uint32_t StreamIn::readLine(char* buffer, uint32_t bufferSize)
{
    uint32_t readCount = 0;
    char* buf = (char*)buffer;
    char ch;
    while(!isEos() && (readCount < bufferSize)) {
        read(ch);
        if(ch == 0x0d) {
            read(ch);
            // todo test if 0x0a
            break;
        }
        *buf = ch;
        ++buf;
        ++readCount;
    }
    *buf = 0;
    return readCount;
}
/*
inline void StreamIn::readSizeString(c::String& result)
{
    size_t size = read<uint32>();
    //result.reserve(size + 1);
    read(const_cast<char*>(result.c_str()), size);
}*/

template<typename T>
inline T StreamIn::read()
{
    T value;
    read(&value, sizeof(T));
    return value;
}

template<typename T>
inline void StreamIn::read(T& value)
{
    read(&value, sizeof(T));
}

} // fileos namespace

#endif
