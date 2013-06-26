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
#include <windows.h>

namespace {

void convertPath(char* dst, int dstLength, wchar_t const* src, int srcLength)
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
}

}

namespace fileos {

struct FileSystem::WatchInfo
{
    WatchInfo(char const* path, bool recursive)
    {
        m_handle = ::CreateFile(path, FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

        if(m_handle != INVALID_HANDLE_VALUE) {
            m_overlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
            m_filter = FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME;
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

            while(!HasOverlappedIoCompleted(&m_overlapped)) {
                SleepEx(1, TRUE);
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
        return ::ReadDirectoryChangesW(
            m_handle,
            m_buffer,
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

        char filename[MAX_PATH];
        WatchInfo* watch = reinterpret_cast<WatchInfo*>(lpOverlapped);
        size_t offset = 0;
        PFILE_NOTIFY_INFORMATION notify = nullptr;
        do {
            notify = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(&watch->m_buffer[offset]);
            offset += notify->NextEntryOffset;

            convertPath(filename, MAX_PATH, notify->FileName, notify->FileNameLength);

            fileos::FileOperation operation;
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

            watch->m_callback(watch->m_id, filename, operation);
        } while(notify->NextEntryOffset != 0);

        if(!watch->m_isStopping)
            watch->registerCompletionRoutine(true);
    }

private:
    friend class FileSystem;

    enum { buffer_size = 16 * 1024 };

    OVERLAPPED m_overlapped;
    HANDLE m_handle;

    BYTE m_buffer[buffer_size];
    DWORD m_filter;
    uint32_t m_id;

    FileSystem::FileModifiedCB* m_callback;

    bool m_isRecursive;
    bool m_isStopping;
};
//-----------------------------------------------------------------------------

FileSystem::FileSystem()
{
}

FileSystem::~FileSystem()
{
}

StreamIn* FileSystem::openForRead(char const* filename)
{
    return FileIn::open(filename);
}

StreamOut* FileSystem::openForWrite(char const* filename, bool append)
{
    return FileOut::open(filename, append);
}

bool FileSystem::fileExists(char const* filename) const
{
    DWORD dwAttrib = ::GetFileAttributes(filename);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool FileSystem::deleteFile(char const* filename)
{
    return ::DeleteFile(filename) != FALSE;
}

bool FileSystem::pathExists(char const* path) const
{
    DWORD dwAttrib = ::GetFileAttributes(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool FileSystem::createPath(char const* path)
{
    return ::CreateDirectory(path, NULL) != FALSE;
}

bool FileSystem::deletePath(char const* path)
{
    return ::RemoveDirectory(path) != FALSE;
}

void FileSystem::findFiles(char const* path, containos::List<char*>& foundFiles)
{
#if 0
    WIN32_FIND_DATA findData;

    Path searchString = path + "/*.*";

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

uint32_t FileSystem::watchFolder(char const* path, FileModifiedCB* callback, bool recursive)
{
    WatchInfo* watch = new WatchInfo(path, recursive);
    uint32_t id = (uint32_t)m_watchList.insert(watch);
    watch->m_id = id;
    watch->m_callback = callback;
    return id;
}

void FileSystem::unwatchFolder(uint32_t id)
{
    WatchInfo* watch = m_watchList[id];
    if(watch == nullptr)
        return;
    delete watch;
    m_watchList[id] = nullptr;
}

void FileSystem::waitForChanges(uint32_t timeoutMs)
{
    DWORD count = (DWORD)m_watchList.size();
    HANDLE* handles = reinterpret_cast<HANDLE*>(alloca(sizeof(HANDLE) * count));
    size_t i = 0;
    for(WatchList::iterator it = m_watchList.begin(); it != m_watchList.end(); ++it)
        handles[i++] = (*it)->m_handle;
    MsgWaitForMultipleObjectsEx(count, handles, timeoutMs, QS_ALLINPUT, MWMO_ALERTABLE);
}

} // end of namespace
