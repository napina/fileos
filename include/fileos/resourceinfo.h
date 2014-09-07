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
#ifndef fileos_resourceinfo_h
#define fileos_resourceinfo_h

#include "fileos/common.h"
#include "fileos/fileinfo.h"
#include "fileos/path.h"
#include "reflectos.h"

namespace fileos {

class ResourceManager;

class ResourceInfo
{
public:
    ResourceInfo();

    virtual ~ResourceInfo() {}

    Path const& filename() const;
    uint64_t fileSize() const;
    FileTime const& lastWriteTime() const;
    ResourceInfo const* const* dependencies() const;
    size_t dependencyCount() const;

protected:
    friend class ResourceManager;

    FileInfo m_fileinfo;
    typeid_t m_resourceType;
    ResourceInfo** m_dependencies;
    size_t m_dependencyCount;

    REFLECT_BASE_CLASS(ResourceInfo, 1)
        REFLECT_FUNCTION(fileSize)
    REFLECT_END()
};

} // end of fileos

#include "fileos/resourceinfo.inl"

#endif
