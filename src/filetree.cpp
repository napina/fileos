/*=============================================================================

Copyright (c) 2014 Ville Ruusutie

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
#include "fileos/filetree.h"

namespace fileos {

FileTree::FileTree(size_t capasity)
    : m_fileInfos(capasity)
{
}

FileTree::~FileTree()
{
}

void FileTree::addFile(FileInfo const* fileInfo)
{
    if(findFile(fileInfo->filename) != nullptr)
        return;

    FileInfo& info = m_fileInfos.acquire();
    info.filename.clone(fileInfo->filename);
    info.timeCreated = fileInfo->timeCreated;
    info.timeModified = fileInfo->timeModified;
    info.fileSize = fileInfo->fileSize;
    info.isReadOnly = fileInfo->isReadOnly; 
    info.isHidden = fileInfo->isHidden;
    info.isTemporary = fileInfo->isTemporary;
    info.isDeleted = fileInfo->isDeleted;
}

void FileTree::removeFile(Path const& path)
{
    c::List<FileInfo>::iterator it = m_fileInfos.begin();
    c::List<FileInfo>::iterator end = m_fileInfos.end();
    for(; it != end; ++it) {
        if(it->filename == path) {
            m_fileInfos.remove(it);
            return;
        }
    }
}

FileInfo const* FileTree::findFile(Path const& path) const
{
    c::List<FileInfo>::const_iterator it = m_fileInfos.begin();
    c::List<FileInfo>::const_iterator end = m_fileInfos.end();
    for(; it != end; ++it) {
        if(it->filename == path) {
            return it;
        }
    }
    return nullptr;
}

void FileTree::addFolder(Path const& /*path*/)
{
}

void FileTree::removeFolder(Path const& /*path*/)
{
}

} // end of fileos
