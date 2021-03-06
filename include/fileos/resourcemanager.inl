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
#ifndef fileos_resourcemanager_inl
#define fileos_resourcemanager_inl

#include "fileos/filein.h"
#include "reflectos.h"

namespace fileos {

template<typename T>
__forceinline T* ResourceManager::acquireResourceAs(Path const& filename)
{
    Resource* resource = acquireResource(filename);
    fileos_assert(resource != nullptr);
    fileos_assert(resource->getTypeInfo()->id() == reflectos::type_inspect<T>::id());
    return static_cast<T*>(resource);
}

template<typename T>
__forceinline T* ResourceManager::acquireResourceAs(resourceid_t id)
{
    Resource* resource = acquireResource(id);
    fileos_assert(resource != nullptr);
    fileos_assert(resource->getTypeInfo()->id() == reflectos::type_inspect<T>::id());
    return static_cast<T*>(resource);
}

template<typename Type,typename InfoType>
__forceinline void ResourceManager::registerType(char const* extension)
{
    registerType(extension, reflectos::type_inspect<Type>::type(), reflectos::type_inspect<InfoType>::type());
}

} // end of fileos

#endif
