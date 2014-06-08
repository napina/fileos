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
//#include "fileos/thread.h"
#include "fileos/filesystem.h"
#include "fileos/filein.h"
#include "containos/hash.h"

namespace fileos {

#if 0
struct ResourceManager::LoaderThread : public Thread
{
    LoaderThread()
        : k::Thread("ResourceLoader", 32 * 1024)
    {
    }

    virtual ~LoaderThread()
    {
    }

    virtual int main()
    {
        while(!isExiting()) {
            // TODO handle loading
            sleep(500);
        }
        return 0;
    }

private:
};
#endif

ResourceManager::~ResourceManager()
{
    //m_loaderThread->waitUntilFinished();
}

ResourceManager::ResourceManager()
    : m_resources(64)
    , m_filenames(64)
    , m_fileSystem()
    , m_rootPath()
    , m_hotloadId(0)
    , m_hotload(false)
{
    //m_loaderThread = k_new(LoaderThread)();
    //m_loaderThread->start();
}

void ResourceManager::update()
{
    m_fileSystem.waitForChanges(0);

    for(size_t i = 0; i < m_pending.size(); ++i) {
        Resource* resource = m_pending[i];
        Path const& filename = *m_filenames.find(resource->id());

        containos::Ref<StreamIn> stream = FileIn::open(filename.c_str());
        if(!stream.isValid())
            continue;

        resource->load(*this, *stream);

        // TODO handle this better
        m_pending.remove(i);
        --i;
    }
}

void ResourceManager::setRoot(utf8_t const* path)
{
    m_rootPath = path;
}

void ResourceManager::enableHotloading(bool enable)
{
    if(m_hotload == enable)
        return;

    m_hotload = enable;
    if(m_hotload) {
        m_hotloadId = m_fileSystem.watchFolder(m_rootPath, fastdelegate::MakeDelegate(this, &ResourceManager::FileModified), true);
    } else {
        m_fileSystem.unwatchFolder(m_hotloadId);
    }
}

void ResourceManager::FileModified(uint32_t id, Path const& filename, FileOperation operation, FileTime& timestamp)
{
    if(operation != fileoperation_modified)
        return;

    Path relativeFilename = filename.relativeTo(m_rootPath);
    resourceid_t fileId = containos::hash32(relativeFilename.c_str(), relativeFilename.length());
    ResourceMap::iterator ite = m_resources.find(fileId);
    if(ite == m_resources.end())
        return;

    for(size_t i = 0; i < m_pending.size(); ++i) {
        if(m_pending[i] == *ite) {
            // already in list so no need to add it
            return;
        }
    }
    m_pending.insert(*ite);

    id;
    timestamp;
    //info(k::Format("Resource '") % filename.c_str() % "' updated");
}

bool ResourceManager::unacquireResource(Resource* resource)
{
    resource;
    return false;
}

bool ResourceManager::hasPendingWork() const
{
    return m_pending.size() > 0;
}

} // end of fileos