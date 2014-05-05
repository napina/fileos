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
#pragma once
#ifndef fileos_resourcemanager_h
#define fileos_resourcemanager_h

#include "fileos/common.h"
#include "fileos/resource.h"
#include "fileos/filesystem.h"
#include "containos/hashmap.h"
#include "containos/ptr.h"

namespace fileos {

class ResourceManager
{
public:
    ~ResourceManager();
    ResourceManager();

    void update();

    void setRoot(utf8_t const* path);
    void enableHotloading(bool enable);

    template<typename T>
    T* acquireResource(utf8_t const* name, size_t cost);
    bool unacquireResource(Resource* resource);

    bool hasPendingWork() const;

private:
    void FileModified(uint32_t id, Path const& filename, FileOperation operation, FileTime& timestamp);

private:
    typedef containos::HashMap<resourceid_t, Resource*> ResourceMap;
    typedef containos::HashMap<resourceid_t, Path> FilenameMap;
    typedef containos::List<Resource*> ResourceList;
    ResourceMap m_resources;
    FilenameMap m_filenames;
    ResourceList m_pending;

    FileSystem m_fileSystem;
    Path m_rootPath;
    uint32_t m_hotloadId;
    bool m_hotload;
    
    struct LoaderThread;
    //containos::Ptr<LoaderThread> m_loaderThread;
};

} // end of fileos

#include "fileos/resourcemanager.inl"

#endif
