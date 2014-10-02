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

#include "fileos/common.h"
#include "containos/utf8.h"

namespace fileos {

class Path
{
public:
    typedef containos::Utf8 Utf8;
    typedef containos::Utf8Slice Utf8Slice;

    Path();
    Path(size_t capasity);
    Path(Path const& other);
    //template<typename T> explicit Path(T const* path);
    template<typename T> explicit Path(T const* path, size_t count);
    template<size_t Count> explicit Path(char const (&str)[Count]);
    template<size_t Count> explicit Path(wchar_t const (&str)[Count]);
    ~Path();

    void reserve(size_t capasity);
    void clear();

    void set(Path const& path);
    void set(Utf8Slice const& slice);
    template<typename T> void set(T const* path);
    template<typename T> void set(T const* path, size_t count);

    void append(Path const& path);
    void append(Utf8Slice const& slice);
    template<typename T> void append(T const* path);
    template<typename T> void append(T const* path, size_t count);

    template<typename T> void convertTo(T* buffer, size_t count) const;

    Path changeExtension(char const* extension) const;
    Utf8Slice relativeTo(Path const& base) const;
    void clone(Path const& from);
    void trimFolders();

    Path parent() const;
    Utf8Slice drive() const;
    Utf8Slice filename() const;
    Utf8Slice extension() const;
    uint8_t const* data() const;
    size_t length() const;
    bool isRelativeTo(Path const& base) const;

    bool operator==(Path const& other) const;
    bool operator==(containos::Utf8Slice const& slice) const;
    bool operator==(char const* str) const;
    bool operator==(wchar_t const* str) const;

private:
    Path(Utf8 const& buffer);
    void fixSlashes();

    Utf8 m_buffer;
};

} // end of fileos

#include "fileos/path.inl"

#endif
