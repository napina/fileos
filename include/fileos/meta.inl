/*===============================================================================

Copyright (c) 2013 Ville Ruusutie

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

===============================================================================*/
#pragma once
#ifndef fileos_meta_inl
#define fileos_meta_inl

namespace fileos {
namespace internal {

template<typename T>
__forceinline int offsetTo(T const* ptr, T const* basePtr)
{
    uintptr_t diff = ptr - basePtr;
    return (int)diff;
}

} // end of internal
//----------------------------------------------------------------------------

inline void MetaNode::setAsValue(char const* typeName, char const* name, char const* value)
{
    m_type = typeName;
    m_nameOffset = internal::offsetTo(name, typeName);
    m_valueOffset = internal::offsetTo(value, typeName);
    m_childOffset = 0;
    m_nextOffset = 0;
}

inline void MetaNode::setAsList(char const* typeName, char const* name, MetaNode const* child)
{
    m_type = typeName;
    m_nameOffset = internal::offsetTo(name, typeName);
    m_valueOffset = 0;
    m_childOffset = (child != nullptr) ? internal::offsetTo(child, this) : 0;
    m_nextOffset = 0;
}

inline void MetaNode::setNext(MetaNode const* next)
{
    m_nextOffset = (next != nullptr) ? internal::offsetTo(next, this) : 0;
}

__forceinline bool MetaNode::isListType() const
{
    return m_valueOffset == 0;
}

__forceinline bool MetaNode::isValueType() const
{
    return m_valueOffset != 0;
}

__forceinline bool MetaNode::hasValue() const
{
    return isValueType() && (value()[0] != 0);
}

__forceinline char const* MetaNode::typeName() const
{
    return m_type;
}

__forceinline char const* MetaNode::name() const
{
    return m_type + m_nameOffset;
}

__forceinline char const* MetaNode::value() const
{
    return m_type + m_valueOffset;
}

template<typename T>
__forceinline T MetaNode::valueAs() const
{
    return parse<T>(value());
}

__forceinline MetaNode const* MetaNode::firstChild() const
{
    if(m_childOffset != 0)
        return this + m_childOffset;
    return nullptr;
}

__forceinline MetaNode const* MetaNode::next() const
{
    if(m_nextOffset != 0)
        return this + m_nextOffset;
    return nullptr;
}

//----------------------------------------------------------------------------

} // end of fileos

#endif
