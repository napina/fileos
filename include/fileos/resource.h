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
#ifndef fileos_resource_h
#define korppu_resource_h

#include "fileos/common.h"
#include "fileos/streamin.h"
#include "containos/event.h"
#include "containos/ref.h"

namespace fileos {

typedef uint32_t resourceid_t;
class ResourceManager;

class Resource
{
public:
    enum State {
        state_notready = 0,
        state_pending,
        state_processing,
        state_ready
    };

    Resource();
    virtual ~Resource();

    virtual bool load(ResourceManager& manager, StreamIn& stream) = 0;

    State state() const;
    resourceid_t id() const;

protected:
    friend class ResourceManager;
    resourceid_t m_id;
    uint32_t m_state : 16;
    REF_STORAGE_BITS(Resource,uint32_t,16);

public:
    containos::Event<void(Resource*)> onLoaded;
};

} // end of fileos

#include "fileos/resource.inl"

#endif
