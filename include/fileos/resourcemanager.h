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
//#include "fileos/filesystem.h"
#include "fileos/path.h"
#include "containos/hashmap.h"
#include "reflectos.h"

namespace fileos {

class Resource;
class ResourceInfo;
class FileSystem;

class ResourceManager
{
public:
    ~ResourceManager();
    ResourceManager(FileSystem* fileSystem);

    void update();
    void collectGarbage();

    bool hasPendingWork() const;

    // Prefer resource ids when accessing resources. 
    template<typename T> T* acquireResourceAs(resourceid_t id);
    Resource* acquireResource(resourceid_t id);
    bool unacquireResource(Resource* resource);
    ResourceInfo* getResourceInfo(resourceid_t id);

    // Use filenames only if you want dynamic behaviour
    template<typename T> T* acquireResourceAs(Path const& filename);
    Resource* acquireResource(Path const& filename);

    template<typename Type,typename InfoType>
    void registerType(char const* extension);
    void registerType(char const* extension, reflectos::TypeInfo const* typeinfo, reflectos::TypeInfo const* infoTypeinfo);

private:
    Resource* createResource(ResourceInfo* info, resourceid_t id);

    typedef containos::HashMap<resourceid_t, Resource*> ResourceMap;
    typedef containos::HashMap<resourceid_t, ResourceInfo*> ResourceInfoMap;
    ResourceMap m_resources;
    ResourceInfoMap m_resourceInfos;
    
    ResourceList m_resourcesToLoad;
    ResourceList m_resourcesToFinish;
    ResourceList m_resourcesToUnload;
    ResourceList m_resourcesInMemory;
    ResourceList m_resourcesFailed;

    FileSystem* m_fileSystem;

    typedef containos::HashMap<typeid_t,reflectos::TypeInfo const*> IDToTypeInfo;
    typedef containos::HashMap<uint32_t,reflectos::TypeInfo const*> HashToTypeInfo;
    IDToTypeInfo m_resourceTypes;
    HashToTypeInfo m_resourceInfoTypes;

    REFLECT_CLASS(ResourceManager, 1)
        REFLECT_FUNCTION(hasPendingWork)
    REFLECT_END()
};

} // end of fileos

#include "fileos/resourcemanager.inl"

#endif
