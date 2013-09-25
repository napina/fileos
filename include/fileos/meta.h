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
#ifndef fileos_meta_h
#define fileos_meta_h

#include "fileos/config.h"

namespace fileos {

class StreamIn;

// Do specialization for supported types
template<typename T>
T parse(char const* str);

//----------------------------------------------------------------------------

class MetaNode
{
public:
    void setAsValue(char const* typeName, char const* name, char const* value);
    void setAsList(char const* typeName, char const* name, MetaNode const* child);
    void setNext(MetaNode const* next);

    bool isListType() const;
    bool isValueType() const;
    bool hasValue() const;
    char const* typeName() const;
    char const* name() const;
    char const* value() const;
    template<typename T> T valueAs() const;
    MetaNode const* next() const;
    MetaNode const* firstChild() const;
    MetaNode const* findChild(char const* name) const;

private:
    char const* m_type;
    int32_t m_nameOffset : 8;
    int32_t m_valueOffset : 8;
    int32_t m_childOffset : 8;
    int32_t m_nextOffset : 8;
#ifdef FILEOS_ARCH64
    int32_t m_padding;
#endif
};
//----------------------------------------------------------------------------

class MetaParser
{
    enum { max_depth = 32  };
    enum { max_nodes = 256 };
public:
    MetaParser();
    ~MetaParser();

    // Will generate node hierarchy. Caller must keep buffer alive for node access.
    bool parse(char* buffer, size_t length);
    bool parse(char const* buffer, size_t length);
    bool parse(StreamIn& stream);

    MetaNode const* first() const;
    MetaNode const* find(const char* name) const;

private:
    char* m_buffer;
    MetaNode* m_nodes;
};
//----------------------------------------------------------------------------

} // end of namespace

#include "fileos/meta.inl"

#endif
