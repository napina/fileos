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
#include "fileos/filesystem.h"
#include "fileos/fileout.h"
#include "fileos/filein.h"
#include "fileos/path.h"
#include "containos/hash.h"
#include <windows.h>

namespace c = containos;

namespace {

/*void convertPath(char* dst, int dstLength, wchar_t const* src, int srcLength)
{
    int count = ::WideCharToMultiByte(
        CP_ACP,
        0,
        src,
        srcLength / sizeof(wchar_t),
        dst,
        dstLength,
        NULL,
        NULL);
    dst[count] = 0;
}*/

fileos::FileTime getFileTime()
{
    fileos::FileTime result;
    SYSTEMTIME systemTime;
    ::GetSystemTime(&systemTime);
    result.year = systemTime.wYear;
    result.month = systemTime.wMonth;
    result.dayOfWeek = systemTime.wDayOfWeek;
    result.day = systemTime.wDay;
    result.hour = systemTime.wHour;
    result.minute = systemTime.wMinute;
    result.second = systemTime.wSecond;
    result.milliseconds = systemTime.wMilliseconds;
    return result;
}

}

namespace fileos {

struct FileSystem::WatchInfo
{
    WatchInfo(utf8_t const* path, bool recursive)
    {
        m_handle = ::CreateFileA(path, FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

        if(m_handle != INVALID_HANDLE_VALUE) {
            ::memset(&m_overlapped, 0, sizeof(OVERLAPPED));
            m_overlapped.hEvent = ::CreateEventA(NULL, TRUE, FALSE, NULL);
            m_bufferOffset = 0;
            m_filter = FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME;
            m_id = 0;
            m_isRecursive = recursive;
            m_isStopping = false;
            registerCompletionRoutine(true);
        }
    }

    ~WatchInfo()
    {
        if(m_handle != INVALID_HANDLE_VALUE) {

            m_isStopping = true;
            ::CancelIo(m_handle);
            registerCompletionRoutine(false);

            if(!HasOverlappedIoCompleted(&m_overlapped)) {
                ::SleepEx(1, TRUE);
            }

            ::CloseHandle(m_overlapped.hEvent);
            ::CloseHandle(m_handle);
        }
    }

    bool isValid()
    {
        return m_handle != INVALID_HANDLE_VALUE;
    }

    bool registerCompletionRoutine(bool enable)
    {
        m_bufferOffset ^= buffer_size;
        return ::ReadDirectoryChangesW(
            m_handle,
            m_buffer + m_bufferOffset,
            buffer_size,
            m_isRecursive,
            m_filter,
            NULL,
            &m_overlapped,
            enable ? completionCallback : NULL) != FALSE;
    }

    static void CALLBACK completionCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
    {
        if(dwErrorCode != ERROR_SUCCESS)
            return;

        if(dwNumberOfBytesTransfered == 0)
            return;

        WatchInfo* watch = reinterpret_cast<WatchInfo*>(lpOverlapped);
        size_t offset = watch->m_bufferOffset;
        PFILE_NOTIFY_INFORMATION notify = nullptr;
        do {
            notify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&watch->m_buffer[offset]);
            offset += notify->NextEntryOffset;

            fileos::FileOperation operation = fileos::fileoperation_modified;
            switch(notify->Action)
            {
            case FILE_ACTION_RENAMED_NEW_NAME:
            case FILE_ACTION_ADDED:
                operation = fileos::fileoperation_added;
                break;
            case FILE_ACTION_RENAMED_OLD_NAME:
            case FILE_ACTION_REMOVED:
                operation = fileos::fileoperation_deleted;
                break;
            case FILE_ACTION_MODIFIED:
                operation = fileos::fileoperation_modified;
                break;
            };

            Path filename(notify->FileName, notify->FileNameLength / sizeof(wchar_t));
            FileTime timestamp = getFileTime();

            watch->m_callback(watch->m_id, filename, operation, timestamp);
        } while(notify->NextEntryOffset != 0);

        if(watch->m_isStopping)
            return;

        watch->registerCompletionRoutine(true);
    }

private:
    friend class FileSystem;

    enum { buffer_size = 8 * 1024 };

    OVERLAPPED m_overlapped; // this needs to be first so that we can access this
    HANDLE m_handle;

    BYTE m_buffer[buffer_size * 2];
    uint32_t m_bufferOffset;
    DWORD m_filter;
    uint32_t m_id;

    FileSystem::EventType m_callback;

    bool m_isRecursive;
    bool m_isStopping;
};
//-----------------------------------------------------------------------------

FileSystem::FileSystem()
{
}

FileSystem::~FileSystem()
{
    for(WatchList::iterator it = m_watchList.begin(); it != m_watchList.end(); ++it) {
        delete(*it);
    }
}

StreamIn* FileSystem::openForRead(utf8_t const* filename)
{
    return FileIn::open(filename);
}

StreamOut* FileSystem::openForWrite(utf8_t const* filename, bool append)
{
    return FileOut::open(filename, append);
}

bool FileSystem::fileExists(utf8_t const* filename) const
{
    DWORD dwAttrib = ::GetFileAttributesA(filename);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool FileSystem::copyFile(utf8_t const* filename, utf8_t const* target)
{
    c::Ref<StreamIn> fileIn = openForRead(filename);
    if(!fileIn.isValid())
        return false;

    if(!pathExists(target)) {
        createPath(target);
    }
    c::Ref<StreamOut> fileOut = openForWrite(target, false);
    if(fileOut.isValid())
        return false;

    fileos_todo("implement copy file");
    return false;
}

bool FileSystem::deleteFile(utf8_t const* filename)
{
    return ::DeleteFileA(filename) != FALSE;
}

bool FileSystem::pathExists(utf8_t const* path) const
{
    DWORD dwAttrib = ::GetFileAttributesA(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool FileSystem::createPath(utf8_t const* path)
{
    return ::CreateDirectoryA(path, NULL) != FALSE;
}

bool FileSystem::deletePath(utf8_t const* path)
{
    return ::RemoveDirectoryA(path) != FALSE;
}

void FileSystem::findFiles(utf8_t const* path, containos::List<utf8_t*>& foundFiles)
{
    path;
    foundFiles;
#if 0
    WIN32_FIND_DATA findData;

    Path searchString = path + "*.*";

    HANDLE handle = ::FindFirstFile(searchString.c_str(), &findData);
    if(handle == INVALID_HANDLE_VALUE)
        return;

    do 
    {
        if(findData.cFileName[0] != '.')
            continue;
        if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            continue;

        Path fullName = path + "/" + findData.cFileName;
        foundFiles.insert(fullName.c_str());
    } while(::FindNextFile(handle, &findData));

    ::FindClose(handle);
#endif
}

uint32_t FileSystem::watchFolder(Path const& path, FileModifiedCB callback, bool recursive)
{
    WatchInfo* watch = new WatchInfo(path.c_str(), recursive);
    uint32_t id = c::hash32(path.c_str());
    watch->m_id = id;
    watch->m_callback.add(callback);
    m_watchList.insert(watch);
    return id;
}

void FileSystem::unwatchFolder(uint32_t id)
{
    for(WatchList::iterator it = m_watchList.begin(); it != m_watchList.end(); ++it) {
        WatchInfo* info = *it;
        if(info->m_id != id)
            continue;
        m_watchList.remove(it);
        delete info;
        return;
    }
}

void FileSystem::waitForChanges(uint32_t timeoutMs)
{
    DWORD count = (DWORD)m_watchList.size();
    HANDLE* handles = reinterpret_cast<HANDLE*>(alloca(sizeof(HANDLE) * count));
    int i = 0;
    for(WatchList::iterator it = m_watchList.begin(); it != m_watchList.end(); ++it) {
        handles[i++] = (*it)->m_handle;
    }
    ::MsgWaitForMultipleObjectsEx(count, handles, timeoutMs, QS_ALLINPUT, MWMO_ALERTABLE);
}

} // end of fileos
