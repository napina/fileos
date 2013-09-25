/*=============================================================================

Copyright (c) 2013 Ville Ruusutie

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
#define _CRT_SECURE_NO_WARNINGS
#include "fileos/meta.h"
#include "fileos/streamin.h"
#include "string.h"

namespace {

__forceinline bool isWhitespace(char ch)
{
    return ch <= 0x20;
}

__forceinline bool isLetter(char ch)
{
    // TODO
    return ch > 0x20;
}

__forceinline char* skipWhitespace(char* text, char const* end)
{
    while(text != end) {
        if(!isWhitespace(*text))
            return text;
        ++text;
    }
    return nullptr;
}

__forceinline char* skipLetters(char* text, char const* end)
{
    while(text != end) {
        if(!isLetter(*text))
            return text;
        ++text;
    }
    return nullptr;
}

__forceinline char* skipUntil(char* text, char const* end, char ch)
{
    while(text != end) {
        if((*text) == ch)
            return text;
        ++text;
    }
    return nullptr;
}

} // end of namespace

namespace fileos {

MetaNode const* MetaNode::findChild(const char* name) const
{
    MetaNode const* node = firstChild();
    while(node != nullptr) {
        if(::strcmp(node->name(), name) == 0)
            break;
        node = node->next();
    }
    return node;
}

MetaParser::MetaParser()
{
    m_nodes = new MetaNode[max_nodes];
    m_nodes->setAsValue(nullptr, nullptr, nullptr);
    m_buffer = nullptr;
}

MetaParser::~MetaParser()
{
    delete [] m_nodes;
    delete [] m_buffer;
}

bool MetaParser::parse(StreamIn& stream)
{
    uint32_t length = uint32_t(stream.size());
    m_buffer = new char[length];
    stream.read(m_buffer, length);
    return parse(m_buffer, length);
}

bool MetaParser::parse(char const* buffer, size_t length)
{
    m_buffer = new char[length];
    ::memcpy(m_buffer, buffer, length);
    return parse(m_buffer, length);
}

bool MetaParser::parse(char* text, size_t length)
{
    MetaNode* nodeStack[max_depth];
    int nodeIndex = 0;
    int nodeStackIndex = 0;
    char* curr = text;
    char* end = curr + length;
    char const* typeName;
    char const* name;
    char const* value;

    curr = ::skipWhitespace(curr, end);

    // format: type name [value]
    nodeStack[0] = nullptr;
    while(curr != end) {
        MetaNode* node = &m_nodes[nodeIndex++];
        MetaNode* previousMetaNode = nodeStack[nodeStackIndex];
        if(previousMetaNode) previousMetaNode->setNext(node);
        nodeStack[nodeStackIndex] = node;

        // parse type
        typeName = curr;
        curr = ::skipLetters(curr, end);
        if(curr == nullptr) return false;
        (*curr) = 0;

        curr = ::skipWhitespace(curr, end);
        if(curr == nullptr) return false;

        // parse name
        name = curr;
        curr = ::skipLetters(curr, end);
        if(curr == nullptr) return false;
        (*curr) = 0;

        curr = ::skipWhitespace(curr, end);
        if(curr == nullptr) return false;
        if(*curr != '[') return false;
        ++curr;

        if(typeName[0] == '#') {
            // parse as list
            char const* helper = ::skipWhitespace(curr, end);
            MetaNode const* childMetaNode = ((*helper) != ']') ? &m_nodes[nodeIndex] : nullptr;
            node->setAsList(typeName+1, name, childMetaNode);
            nodeStack[++nodeStackIndex] = nullptr;
        } else {
            // parse as value
            value = curr;
            curr = ::skipUntil(curr, end, ']');
            if(curr == nullptr) return false;
            (*curr) = 0;

            node->setAsValue(typeName, name, value);
        }

        curr = ::skipWhitespace(curr, end);
        if(curr == nullptr) break;

        while((*curr) == ']') {
            // end of list
            --nodeStackIndex;
            ++curr;
            curr = ::skipWhitespace(curr, end);
            if(curr == nullptr) return true;
        }
    }
    return true;
}

MetaNode const* MetaParser::first() const
{
    return &m_nodes[0];
}

MetaNode const* MetaParser::find(const char* name) const
{
    MetaNode const* node = first();
    while(node != nullptr) {
        if(::strcmp(node->name(), name) == 0)
            return node;
        node = node->next();
    }
    return nullptr;
}

} // end of fileos
