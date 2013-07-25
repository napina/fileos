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
#ifndef fileos_path_h
#define fileos_path_h

#include "fileos/config.h"

namespace fileos {

class Path
{
public:
    Path();
    Path(wchar_t const* path);
    Path(wchar_t const* path, size_t aLength);
    Path(Path const& other);
    ~Path();

    Path const& catenate(Path const& a, Path const& b);
    Path const& catenate(Path const& a, wchar_t const* b);
    Path const& catenate(wchar_t const* a, Path const& b);

    wchar_t const* c_str() const;
    size_t length() const;

    // friends
    friend Path catenate(Path const& a, Path const& b);
    friend Path catenate(Path const& a, wchar_t const* b);
    friend Path catenate(wchar_t const* a, Path const& b);

private:
    void construct(wchar_t const* a, size_t aLength);
    void construct(wchar_t const* a, size_t aLength, wchar_t const* b, size_t bLength);
    void destruct();

    struct Buffer;
    Buffer* m_buffer;
};

Path catenate(Path const& a, Path const& b);
Path catenate(Path const& a, wchar_t const* b);
Path catenate(wchar_t const* a, Path const& b);

} // end of fileos

#include "fileos/path.inl"

#endif
