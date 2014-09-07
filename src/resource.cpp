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
#include "fileos/resource.h"
#include "containos/atomic.h"

REGISTER_CLASS(fileos::Resource)

namespace fileos {

Resource::Resource()
    : m_id(0)
    , m_state(resourcestate_notready)
    , m_category(0)
    , m_refCount(0)
    , m_info(nullptr)
    , m_list(nullptr)
{
}

Resource::~Resource()
{
}

void Resource::addReference()
{
    containos::atomicInc32(m_refCount);
}

uint32_t Resource::removeReference()
{
    return containos::atomicDec32(m_refCount);
}

} // end of fileos
