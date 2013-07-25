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
#ifndef fileos_filesystem_h
#define fileos_filesystem_h

#include "fileos/config.h"
#include "fileos/path.h"
#include "containos/event.h"
#include "containos/bitblock.h"
#include "containos/list.h"

namespace fileos {

class StreamIn;
class StreamOut;

enum FileOperation {
    fileoperation_added,
    fileoperation_modified,
    fileoperation_deleted,
};

struct FileTime
{
    uint16_t year;
    uint16_t month;
    uint16_t dayOfWeek;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
    uint16_t milliseconds;
};

class FileSystem
{
public:
    FileSystem();
    ~FileSystem();

    StreamIn* openForRead(wchar_t const* filename);
    StreamOut* openForWrite(wchar_t const* filename, bool append);
    //-------------------------------------------------------------------------

    bool fileExists(wchar_t const* filename) const;
    bool deleteFile(wchar_t const* filename);

    bool pathExists(wchar_t const* path) const;
    bool createPath(wchar_t const* path);
    bool deletePath(wchar_t const* path);
    //-------------------------------------------------------------------------

    // TODO iteration functions
    void findFiles(wchar_t const* path, containos::List<wchar_t*>& foundFiles);
    //-------------------------------------------------------------------------

    typedef void FileModifiedCB(uint32_t id, Path const& filename, FileOperation operation, FileTime& timestamp);

    uint32_t watchFolder(Path const& path, FileModifiedCB* callback, bool recursive);
    void unwatchFolder(uint32_t id);
    void waitForChanges(uint32_t timeoutMs);
    //-------------------------------------------------------------------------

protected:
    struct WatchInfo;
    typedef containos::BitBlock<WatchInfo*> WatchList;
    WatchList m_watchList;
};

} // end of fileos

#endif
