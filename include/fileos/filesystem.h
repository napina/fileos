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

typedef uint32_t watch_t;

class FileSystem
{
    typedef containos::Event<void (watch_t handle, Path const& filename, FileOperation operation, FileTime const& timestamp)> EventType;
public:
    FileSystem();
    ~FileSystem();

    StreamIn* openForRead(char const* filename);
    StreamIn* openForRead(wchar_t const* filename);
    StreamIn* openForRead(Path const& filename);
    StreamOut* openForWrite(char const* filename, bool append);
    StreamOut* openForWrite(wchar_t const* filename, bool append);
    StreamOut* openForWrite(Path const& filename, bool append);
    //-------------------------------------------------------------------------

    bool fileExists(char const* filename) const;
    bool fileExists(wchar_t const* filename) const;
    bool fileExists(Path const& filename) const;
    bool queryInfo(char const* filename, FileInfo& info) const;
    bool queryInfo(wchar_t const* filename, FileInfo& info) const;
    bool queryInfo(Path const& filename, FileInfo& info) const;
    bool pathExists(char const* path) const;
    bool pathExists(wchar_t const* path) const;
    bool pathExists(Path const& path) const;
    //-------------------------------------------------------------------------

    bool copyFile(char const* filename, char const* target);
    bool copyFile(wchar_t const* filename, wchar_t const* target);
    bool copyFile(Path const& filename, Path const& target);
    bool deleteFile(char const* filename);
    bool deleteFile(wchar_t const* filename);
    bool deleteFile(Path const& filename);
    //-------------------------------------------------------------------------

    bool createPath(char const* path);
    bool createPath(wchar_t const* path);
    bool createPath(Path const& path);
    bool deletePath(char const* path);
    bool deletePath(wchar_t const* path);
    bool deletePath(Path const& path);
    //-------------------------------------------------------------------------

//    FileIterator findFiles(utf8_t const* path, utf8_t const* filter);
//    FileIterator findDirectories(utf8_t const* path, utf8_t const* filter);
    //-------------------------------------------------------------------------

    typedef EventType::DelegateType FileModifiedCB;
    watch_t watchFolder(char const* path, FileModifiedCB callback, bool recursive);
    watch_t watchFolder(wchar_t const* path, FileModifiedCB callback, bool recursive);
    watch_t watchFolder(Path const&, FileModifiedCB callback, bool recursive);
    void unwatchFolder(watch_t handle);
    void waitForChanges(uint32_t timeoutMs);
    //-------------------------------------------------------------------------

protected:
    struct WatchInfo;
    typedef containos::List<WatchInfo*> WatchList;
    WatchList m_watchList;
};

} // end of fileos

#endif
