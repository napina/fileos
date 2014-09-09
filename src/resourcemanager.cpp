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
#include "fileos/resourcemanager.h"
#include "fileos/resourcestorage.h"
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

ResourceManager::ResourceManager(FileSystem* fileSystem)
    : m_resources(64)
    , m_resourceInfos(64)
    , m_resourcesToLoad()
    , m_resourcesToFinish()
    , m_resourcesToUnload()
    , m_resourcesInMemory()
    , m_resourcesFailed()
    , m_fileSystem(fileSystem)
    , m_resourceTypes(64)
    , m_resourceInfoTypes(64)
{
}

void ResourceManager::update()
{
    Resource* resource = nullptr;
    while(m_resourcesToLoad.pop(resource)) {
        ResourceInfo* info = resource->m_info;
        containos::Ref<StreamIn> stream = m_fileSystem->openForRead(info->filename());
        if(stream.isValid()) {
            resource->m_state = ResourceState::resourcestate_processing;
            if(resource->load(*this, *stream)) {
                m_resourcesToFinish.add(resource);
                continue;
            }
        }
        resource->m_state = ResourceState::resourcestate_failed;
        m_resourcesFailed.add(resource);
    }

    while(m_resourcesToFinish.pop(resource)) {
        if(resource->finishLoad(*this)) {
            resource->m_state = ResourceState::resourcestate_ready;
            m_resourcesInMemory.add(resource);
        } else {
            resource->m_state = ResourceState::resourcestate_failed;
            m_resourcesFailed.add(resource);
        }
    }
}

void ResourceManager::collectGarbage()
{
    for(size_t i = 0; i < m_resourcesInMemory.size(); ++i) {
        Resource* resource = m_resourcesInMemory[i];
        if(resource->m_refCount == 1) {
            resource->m_state = ResourceState::resourcestate_freeing;
            m_resourcesInMemory.remove(resource);
            m_resourcesToUnload.add(resource);
            --i;
        }
    }

    Resource* resource = nullptr;
    while(m_resourcesToUnload.pop(resource)) {
        resource->unload(*this);
        m_resources.remove(resource->id());
        containos_placement_delete(resource, Resource);
        containos::Mallocator::dealloc(resource);
    }
}

#if 0
void ResourceManager::onFileModified(watch_t, Path const& absoluteFilename, FileOperation operation, FileTime const& timestamp)
{
    switch(operation) {
        case fileos::fileoperation_added:
            // add to file info database
            break;
        case fileos::fileoperation_modified:
            {
                Path relativeFilename = absoluteFilename.relativeTo(m_rootPath);
                resourceid_t fileId = containos::hash32(relativeFilename.data());
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
    m_fileSystem->findFiles(m_rootPath.data(), search, foundFiles);

    for(size_t i = 0; i < foundFiles.size(); ++i) {
        resourceid_t id = containos::hash32(foundFiles[i].filename.data());
        ResourceInfo* info = new ResourceInfo();
        info->m_fileinfo = foundFiles[i];
        info->m_resourceType = 0;
        m_resourceInfos.insert(id, info);
    }
}
#endif

Resource* ResourceManager::acquireResource(Path const& filename)
{
    const resourceid_t id = containos::hash32(filename.data());
    Resource* resource = acquireResource(id);
    if(resource != nullptr) {
        return resource;
    }

    // Not found in database. Try to open file and add to database
    FileInfo fileinfo;
    if(m_fileSystem->queryInfo(filename, fileinfo) == false) {
        return nullptr;
    }

    containos::Utf8Slice extension = filename.extension();
    HashToTypeInfo::const_iterator infoIt = m_resourceInfoTypes.find(containos::hash32(extension.m_begin));
    if(infoIt == m_resourceInfoTypes.end()) {
        return nullptr;
    }

    ResourceInfo* resourceInfo = reinterpret_cast<ResourceInfo*>(infoIt->construct());
    resourceInfo->m_fileinfo = fileinfo;
    return createResource(resourceInfo, id);
}

Resource* ResourceManager::acquireResource(resourceid_t id)
{
    ResourceMap::iterator resourceIt = m_resources.find(id);
    if(resourceIt != m_resources.end()){
        resourceIt->addReference();
        return *resourceIt;
    }

    ResourceInfoMap::iterator infoIt = m_resourceInfos.find(id);
    if(infoIt != m_resourceInfos.end()) {
        return createResource(*infoIt, id);
    }
    return nullptr;
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
    m_resourcesToUnload.add(resource);
    return true;
}

bool ResourceManager::hasPendingWork() const
{
    return m_resourcesToLoad.size() > 0;
}

void ResourceManager::registerType(char const* extension, const reflectos::TypeInfo* typeinfo, const reflectos::TypeInfo* infoTypeinfo)
{
    fileos_assert(extension != nullptr);
    fileos_assert(typeinfo != nullptr);
    fileos_assert(typeinfo->isDerivedFrom<Resource>());
    fileos_assert(infoTypeinfo != nullptr);
    fileos_assert(infoTypeinfo->isDerivedFrom<ResourceInfo>());
    m_resourceTypes.insert(typeinfo->id(), typeinfo);
    m_resourceInfoTypes.insert(containos::hash32(extension), infoTypeinfo);
}

Resource* ResourceManager::createResource(ResourceInfo* info, resourceid_t id)
{
    IDToTypeInfo::const_iterator typeIt = m_resourceTypes.find(info->m_resourceType);
    if(typeIt == m_resourceTypes.end()) {
        return nullptr;
    }
    
    Resource* resource = static_cast<Resource*>(typeIt->construct());
    resource->m_id = id;
    resource->m_state = ResourceState::resourcestate_notready;
    resource->m_category = 0;
    resource->m_refCount = 1;
    resource->m_list = nullptr;
    resource->m_info = info;
    m_resources.insert(id, resource);

    if(resource->prepareLoad(*this)) {
        resource->m_state = ResourceState::resourcestate_pending;
        m_resourcesToLoad.add(resource);
    } else {
        resource->m_state = ResourceState::resourcestate_failed;
        m_resourcesFailed.add(resource);
    }
    return resource;
}

} // end of fileos
