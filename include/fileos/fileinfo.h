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
#ifndef fileos_fileinfo_h
#define fileos_fileinfo_h

#include "fileos/common.h"
#include "fileos/path.h"

namespace fileos {

struct FileTime
{
    uint32_t year : 15;
    uint32_t month : 4;
    uint32_t dayOfWeek: 3;
    uint32_t day : 5;
    uint32_t hour : 5;
    uint32_t minute : 6;
    uint32_t second : 6;
    uint32_t milliseconds : 10;
};

struct FileInfo
{
    Path filename;
    FileTime createTime;
    FileTime lastWriteTime;
    uint64_t fileSize;
    uint32_t isReadOnly : 1;
    uint32_t isHidden : 1;
    uint32_t isTemporary : 1;
    uint32_t isDeleted : 1;
};

} // end of fileos

namespace containos {
template<> struct allow_memcpy<fileos::FileInfo> {
    static const bool allowed = false;
};
}

#endif
