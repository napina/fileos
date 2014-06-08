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

#include "fileos/common.h"
#include "fileos/fileinfo.h"
#include "fileos/path.h"
#include "containos/event.h"
#include "containos/list.h"

namespace fileos {

class StreamIn;
class StreamOut;

enum FileOperation {
    fileoperation_added,
    fileoperation_modified,
    fileoperation_deleted,
};

class FileSystem
{
    typedef containos::Event<void (uint32_t id, utf16_t const* filename, FileOperation operation, FileTime& timestamp)> EventType;
public:
    FileSystem();
    ~FileSystem();

    StreamIn* openForRead(utf16_t const* filename);
    StreamOut* openForWrite(utf16_t const* filename, bool append);
    //-------------------------------------------------------------------------

    bool fileExists(utf16_t const* filename) const;
    bool queryInfo(utf16_t const* filename, FileInfo& info) const;
    bool copyFile(utf16_t const* filename, utf16_t const* target);
    bool deleteFile(utf16_t const* filename);
    //-------------------------------------------------------------------------

    bool pathExists(utf16_t const* path) const;
    bool createPath(utf16_t const* path);
    bool deletePath(utf16_t const* path);
    //-------------------------------------------------------------------------

//    FileIterator findFiles(utf8_t const* path, utf8_t const* filter);
//    FileIterator findDirectories(utf8_t const* path, utf8_t const* filter);
    //-------------------------------------------------------------------------

    typedef EventType::DelegateType FileModifiedCB;
    uint32_t watchFolder(utf16_t const* path, FileModifiedCB callback, bool recursive);
    void unwatchFolder(uint32_t id);
    void waitForChanges(uint32_t timeoutMs);
    //-------------------------------------------------------------------------

protected:
    struct WatchInfo;
    typedef containos::List<WatchInfo*> WatchList;
    WatchList m_watchList;
};

} // end of fileos

#endif
