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
#ifndef fileos_resourceinfo_inl
#define fileos_resourceinfo_inl

namespace fileos {

__forceinline Path const& ResourceInfo::filename() const
{
    return m_fileinfo.filename;
}

__forceinline FileTime const& ResourceInfo::lastWriteTime() const
{
    return m_fileinfo.lastWriteTime;
}

__forceinline uint64_t ResourceInfo::fileSize() const
{
    return m_fileinfo.fileSize;
}

__forceinline ResourceInfo const* const* ResourceInfo::dependencies() const
{
    return m_dependencies;
}
__forceinline size_t ResourceInfo::dependencyCount() const
{
    return m_dependencyCount;
}

} // end of fileos

#endif
