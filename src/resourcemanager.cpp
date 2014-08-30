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
//#include "stdafx.h"
#include "fileos/resourcemanager.h"
#include "fileos/resourceinfo.h"
#include "fileos/resource.h"
#include "fileos/filesystem.h"
#include "fileos/fileinfo.h"
#include "fileos/filein.h"
#include "containos/hash.h"

namespace fileos {

ResourceManager::~ResourceManager()
{
}

ResourceManager::ResourceManager()
    : m_resources(64)
    , m_resourceInfos(64)
    , m_fileSystem()
    , m_rootPath()
    , m_hotloadId(0)
    , m_hotload(false)
    , m_resourceTypes(64)
    , m_resourceInfoTypes(64)
{
    registerResourceInfoType<ResourceInfo>();
}

void ResourceManager::update()
{
    m_fileSystem.waitForChanges(10);

    Resource* resource = nullptr;
    while(m_resourcesToLoad.pop(resource)) {
        ResourceInfo* info = resource->m_info;

        containos::Ref<StreamIn> stream = m_fileSystem.openForRead(info->filename());
        if(stream.isValid()) {
            resource->load(*this, *stream);
            resource->finishLoad(*this);
        } else {
            m_resourcesToLoad.add(resource);
        }
    }
    m_resourcesToLoad.clear();

    resource = nullptr;
    while(m_resourcesToUnload.pop(resource)) {
        m_resources.remove(resource->id());
        containos_placement_delete(resource, Resource);
        containos::Mallocator::dealloc(resource);
    }
    m_resourcesToUnload.clear();
}

void ResourceManager::enableHotloading(bool enable)
{
    if(m_hotload == enable)
        return;

    m_hotload = enable;
    if(m_hotload) {
        m_hotloadId = m_fileSystem.watchFolder(m_rootPath, fastdelegate::MakeDelegate(this, &ResourceManager::onFileModified), true);
    } else {
        m_fileSystem.unwatchFolder(m_hotloadId);
    }
}

void ResourceManager::onFileModified(watch_t, Path const& absoluteFilename, FileOperation operation, FileTime const& timestamp)
{
    switch(operation) {
        case fileos::fileoperation_added:
            // add to file info database
            break;
        case fileos::fileoperation_modified:
            {
                Path relativeFilename = absoluteFilename.relativeTo(m_rootPath);
                resourceid_t fileId = containos::hash32(relativeFilename.data(), relativeFilename.length());
                ResourceMap::iterator ite = m_resources.find(fileId);
                if(ite == m_resources.end())
                    return;

                Resource* resource = *ite;
                m_resourcesToLoad.add(resource);
                resource->m_list = &m_resourcesToLoad;

                // todo update full fileinfo
                resource->m_info->m_fileinfo.lastWriteTime = timestamp;
                break;
            }
        case fileos::fileoperation_deleted:
            // update status to not available
            break;
    }
}

void ResourceManager::updateResourceInfos()
{
    uint8_t const* search = (uint8_t const*)"/*";

    containos::List<FileInfo> foundFiles;
    m_fileSystem.findFiles(m_rootPath.data(), search, foundFiles);

    for(size_t i = 0; i < foundFiles.size(); ++i) {
        resourceid_t id = containos::hash32((const char*)foundFiles[i].filename.data());
        ResourceInfo* info = new ResourceInfo();
        info->m_fileinfo = foundFiles[i];
        info->m_resourceType = 0;
        m_resourceInfos.insert(id, info);
    }
}

Resource* ResourceManager::acquireResource(uint8_t const* filename)
{
    resourceid_t id = containos::hash32(filename, containos::countUtfElements(filename));
    return acquireResource(id);
}

Resource* ResourceManager::acquireResource(resourceid_t id)
{
    ResourceMap::iterator resourceIt = m_resources.find(id);
    if(resourceIt != m_resources.end()) {
        resourceIt->addReference();
        return *resourceIt;
    }

    ResourceInfoMap::iterator infoIt = m_resourceInfos.find(id);
    if(infoIt == m_resourceInfos.end())
        return nullptr;
    
    TypeInfoMap::const_iterator typeIt = m_resourceTypes.find(infoIt->m_resourceType);
    if(typeIt == m_resourceTypes.end())
        return nullptr;
    
    Resource* resource = static_cast<Resource*>(typeIt->construct());
    resource->m_id = id;
    resource->m_state = ResourceState::resourcestate_notready;
    resource->m_category = 0;
    resource->m_refCount = 1;
    resource->m_list = nullptr;
    resource->m_info = *infoIt;
    m_resources.insert(id, resource);

    if(resource->prepareLoad(*this)) {
        resource->m_state = ResourceState::resourcestate_pending;
        resource->m_list = &m_resourcesToLoad;
        m_resourcesToLoad.add(resource);
    } else {
        resource->m_state = ResourceState::resourcestate_freeing;
        resource->m_list = &m_resourcesToUnload;
        m_resourcesToUnload.add(resource);
    }
    return resource;
}

bool ResourceManager::unacquireResource(Resource* resource)
{
    if(resource == nullptr)
        return false;
    
    fileos_assert(m_resources.find(resource->id()) == m_resources.end());
    
    uint32_t count = resource->removeReference();
    if(count > 1)
        return true;
        
    if(resource->m_list != nullptr)
        resource->m_list->remove(resource);
    resource->m_list = &m_resourcesToUnload;
    m_resourcesToUnload.add(resource);
    return true;
}

bool ResourceManager::hasPendingWork() const
{
    return m_resourcesToLoad.size() > 0;
}

void ResourceManager::registerResourceType(const reflectos::TypeInfo* typeinfo)
{
    fileos_assert(typeinfo != nullptr);
    fileos_assert(typeinfo->isDerivedFrom<Resource>());
    m_resourceTypes.insert(typeinfo->id(), typeinfo);
}

void ResourceManager::registerResourceInfoType(const reflectos::TypeInfo* typeinfo)
{
    fileos_assert(typeinfo != nullptr);
    fileos_assert(typeinfo->isDerivedFrom<ResourceInfo>());
    m_resourceTypes.insert(typeinfo->id(), typeinfo);
}

} // end of fileos
