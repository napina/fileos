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
#include "pch.h"
#include "fileos/filesystem.h"
#ifdef FILEOS_WINDOWS
#include "fileos/fileout.h"
#include "fileos/filein.h"
#include "fileos/path.h"
#include "containos/hash.h"
#include "timeutils_windows.h"
#include <windows.h>

namespace c = containos;

namespace {

fileos::FileTime getFileTime()
{
    fileos::FileTime result;
    SYSTEMTIME systemTime;
    ::GetSystemTime(&systemTime);
    convertTime(systemTime, result);
    return result;
}

}

namespace fileos {

struct FileSystem::WatchInfo
{
    WatchInfo(char const* path, bool recursive)
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

    WatchInfo(wchar_t const* path, bool recursive)
    {
        m_handle = ::CreateFileW(path, FILE_LIST_DIRECTORY,
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

    FileSystem::FileOperationDelegate m_callback;

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

StreamIn* FileSystem::openForRead(char const* filename)
{
    return FileIn::open(filename);
}

StreamIn* FileSystem::openForRead(wchar_t const* filename)
{
    return FileIn::open(filename);
}

StreamIn* FileSystem::openForRead(Path const& filename)
{
    return FileIn::open(filename);
}

StreamOut* FileSystem::openForWrite(char const* filename, bool append)
{
    return FileOut::open(filename, append);
}

StreamOut* FileSystem::openForWrite(wchar_t const* filename, bool append)
{
    return FileOut::open(filename, append);
}

StreamOut* FileSystem::openForWrite(Path const& filename, bool append)
{
    return FileOut::open(filename, append);
}

bool FileSystem::fileExists(char const* filename) const
{
    DWORD dwAttrib = ::GetFileAttributesA(filename);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool FileSystem::fileExists(wchar_t const* filename) const
{
    DWORD dwAttrib = ::GetFileAttributesW(filename);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool FileSystem::fileExists(Path const& filename) const
{
    wchar_t buffer[1024];
    filename.convertTo(buffer, 1024);
    return fileExists(buffer);
}

bool FileSystem::queryInfo(char const* filename, FileInfo& info) const
{
    WIN32_FILE_ATTRIBUTE_DATA fileInformation;
    if(::GetFileAttributesExA(filename, GetFileExInfoStandard, &fileInformation) == FALSE)
        return false;

    info.filename.set(filename);
    convertTime(fileInformation.ftCreationTime, info.timeCreated);
    convertTime(fileInformation.ftLastWriteTime, info.timeModified);
    info.fileSize = (uint64_t(fileInformation.nFileSizeHigh) << 32) + fileInformation.nFileSizeLow;
    info.isReadOnly = (fileInformation.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
    info.isHidden = (fileInformation.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
    info.isTemporary = (fileInformation.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0;
    info.isDeleted = false;
    return true;
}

bool FileSystem::queryInfo(wchar_t const* filename, FileInfo& info) const
{
    WIN32_FILE_ATTRIBUTE_DATA fileInformation;
    if(::GetFileAttributesExW(filename, GetFileExInfoStandard, &fileInformation) == FALSE)
        return false;

    info.filename.set(filename);
    convertTime(fileInformation.ftCreationTime, info.timeCreated);
    convertTime(fileInformation.ftLastWriteTime, info.timeModified);
    info.fileSize = (uint64_t(fileInformation.nFileSizeHigh) << 32) + fileInformation.nFileSizeLow;
    info.isReadOnly = (fileInformation.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
    info.isHidden = (fileInformation.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
    info.isTemporary = (fileInformation.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0;
    info.isDeleted = false;
    return true;
}

bool FileSystem::queryInfo(Path const& filename, FileInfo& info) const
{
    wchar_t buffer[1024];
    filename.convertTo(buffer, 1024);
    return queryInfo(buffer, info);
}

bool FileSystem::copyFile(char const* filename, char const* target)
{
    BOOL ok = ::CopyFileExA(filename, target, NULL, this, FALSE, 0);
    return ok == TRUE;
}

bool FileSystem::copyFile(wchar_t const* filename, wchar_t const* target)
{
    BOOL ok = ::CopyFileExW(filename, target, NULL, this, FALSE, 0);
    return ok == TRUE;
}

bool FileSystem::copyFile(Path const& filename, Path const& target)
{
    wchar_t srcBuffer[1024];
    wchar_t dstBuffer[1024];
    filename.convertTo(srcBuffer, 1024);
    target.convertTo(dstBuffer, 1024);
    return copyFile(srcBuffer, dstBuffer);
}

bool FileSystem::deleteFile(char const* filename)
{
    return ::DeleteFileA(filename) != FALSE;
}

bool FileSystem::deleteFile(wchar_t const* filename)
{
    return ::DeleteFileW(filename) != FALSE;
}

bool FileSystem::deleteFile(Path const& filename)
{
    wchar_t buffer[1024];
    filename.convertTo(buffer, 1024);
    return deleteFile(buffer);
}

bool FileSystem::pathExists(char const* path) const
{
    DWORD dwAttrib = ::GetFileAttributesA(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool FileSystem::pathExists(wchar_t const* path) const
{
    DWORD dwAttrib = ::GetFileAttributesW(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool FileSystem::pathExists(Path const& path) const
{
    wchar_t buffer[1024];
    path.convertTo(buffer, 1024);
    return pathExists(buffer);
}

bool FileSystem::createPath(char const* path)
{
    return ::CreateDirectoryA(path, NULL) != FALSE;
}

bool FileSystem::createPath(wchar_t const* path)
{
    return ::CreateDirectoryW(path, NULL) != FALSE;
}

bool FileSystem::createPath(Path const& path)
{
    wchar_t buffer[1024];
    path.convertTo(buffer, 1024);
    return createPath(buffer);
}

bool FileSystem::deletePath(char const* path)
{
    return ::RemoveDirectoryA(path) != FALSE;
}

bool FileSystem::deletePath(wchar_t const* path)
{
    return ::RemoveDirectoryW(path) != FALSE;
}

bool FileSystem::deletePath(Path const& path)
{
    return deletePath(reinterpret_cast<wchar_t const*>(path.data()));
}

void FileSystem::findFiles(Path const& path, char const* filter, FileTree& foundFiles)
{
    Path searchString;
    searchString.reserve(1024);
    searchString.append(path);
    searchString.append(filter);

    WIN32_FIND_DATAA findData;
    HANDLE findHandle = ::FindFirstFileA((const char*)searchString.data(), &findData);
    if(findHandle == INVALID_HANDLE_VALUE)
        return;

    // First find directories
    do
    {
        if(findData.cFileName[0] == '.')
            continue;
        if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            Path directoryPath;
            directoryPath.reserve(1024);
            directoryPath.append(path);
            directoryPath.append((char const*)findData.cFileName);
            findFiles(directoryPath, filter, foundFiles);
        }
    } while(::FindNextFileA(findHandle, &findData));
    ::FindClose(findHandle);

    // Then files in folder
    FileInfo fileinfo;
    fileinfo.filename.reserve(1024);
    findHandle = ::FindFirstFileA((const char*)searchString.data(), &findData);
    do
    {
        if(findData.cFileName[0] == '.')
            continue;
        if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            continue;

        fileinfo.filename.clear();
        fileinfo.filename.append(path);
        fileinfo.filename.append(findData.cFileName);
        convertTime(findData.ftCreationTime, fileinfo.timeCreated);
        convertTime(findData.ftLastWriteTime, fileinfo.timeModified);
        fileinfo.fileSize = (uint64_t(findData.nFileSizeHigh) << 32) + findData.nFileSizeLow;
        fileinfo.isReadOnly = (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
        fileinfo.isHidden = (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
        fileinfo.isTemporary = (findData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0;
        fileinfo.isDeleted = false;
        foundFiles.addFile(&fileinfo);
    } while(::FindNextFileA(findHandle, &findData));
    ::FindClose(findHandle);
}

uint32_t FileSystem::watchFolder(char const* path, FileOperationCB callback, bool recursive)
{
    WatchInfo* watch = new WatchInfo(path, recursive);
    uint32_t id = c::hash32(path);
    watch->m_id = id;
    watch->m_callback.set(callback);
    m_watchList.insert(watch);
    return id;
}

uint32_t FileSystem::watchFolder(wchar_t const* path, FileOperationCB callback, bool recursive)
{
    WatchInfo* watch = new WatchInfo(path, recursive);
    uint32_t id = c::hash32(reinterpret_cast<char const*>(path));
    watch->m_id = id;
    watch->m_callback.set(callback);
    m_watchList.insert(watch);
    return id;
}

uint32_t FileSystem::watchFolder(Path const& path, FileOperationCB callback, bool recursive)
{
    wchar_t buffer[1024];
    path.convertTo(buffer, 1024);
    return watchFolder(buffer, callback, recursive);
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

#endif
