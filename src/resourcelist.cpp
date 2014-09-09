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
#include "fileos/resourcelist.h"
#include "fileos/resource.h"

namespace fileos {

ResourceList::ResourceList()
    : m_resources(128)
{
}

void ResourceList::reserve(size_t capasity)
{
    m_resources.reserve(capasity);
}

void ResourceList::add(Resource* resource)
{
    ListType::iterator it = m_resources.begin();
    ListType::iterator end = m_resources.end();
    for(; it != end; ++it) {
        if(*it == resource)
            return;
    }

    m_resources.insert(resource);
    resource->m_list = this;
}

void ResourceList::remove(Resource* resource)
{
    ListType::iterator it = m_resources.begin();
    ListType::iterator end = m_resources.end();
    for(; it != end; ++it) {
        if(*it != resource)
            continue;
        m_resources.remove(it);
        return;;
    }
}

void ResourceList::clear()
{
    return m_resources.clear();
}

bool ResourceList::pop(Resource*& resource)
{
    return m_resources.pop(resource);
}

size_t ResourceList::size() const
{
    return m_resources.size();
}

} // end of fileos
