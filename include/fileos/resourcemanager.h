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
#include "fileos/resourcelist.h"
#include "fileos/filesystem.h"
#include "fileos/path.h"
#include "containos/hashmap.h"
//#include "containos/ptr.h"
//#include "reflectos.h"

namespace reflectos {
    struct TypeInfo;
}

namespace fileos {

class Resource;
class ResourceInfo;

class ResourceManager
{
public:
    ~ResourceManager();
    ResourceManager();

    void update();

    void setRoot(char const* path);
    void setRoot(wchar_t const* path);
    void setRoot(Path const& path);
    void enableHotloading(bool enable);
    bool hasPendingWork() const;

    template<typename T> T* acquireResourceAs(uint8_t const* name);
    template<typename T> T* acquireResourceAs(resourceid_t id);
    Resource* acquireResource(uint8_t const* name);
    Resource* acquireResource(resourceid_t id);
    bool unacquireResource(Resource* resource);

    template<typename T>
    void registerResourceType();
    void registerResourceType(const reflectos::TypeInfo* typeinfo);

    template<typename T>
    void registerResourceInfoType();
    void registerResourceInfoType(const reflectos::TypeInfo* typeinfo);

private:
    void updateResourceInfos();

    void onFileModified(uint32_t id, Path const& filename, FileOperation operation, FileTime const& timestamp);

private:
    typedef containos::HashMap<resourceid_t, Resource*> ResourceMap;
    typedef containos::HashMap<resourceid_t, ResourceInfo*> ResourceInfoMap;
    ResourceMap m_resources;
    ResourceInfoMap m_resourceInfos;
    
    ResourceList m_resourcesToLoad;
    ResourceList m_resourcesInMemory;
    ResourceList m_resourcesToUnload;

    FileSystem m_fileSystem;
    Path m_rootPath;

    // TODO
    uint32_t m_hotloadId;
    bool m_hotload;

    typedef containos::HashMap<typeid_t,reflectos::TypeInfo const*> TypeInfoMap;
    TypeInfoMap m_resourceTypes;
    TypeInfoMap m_resourceInfoTypes;
};

} // end of fileos

#include "fileos/resourcemanager.inl"

#endif
