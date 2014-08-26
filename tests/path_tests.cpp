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
#include "unitos/unitos.h"
#include "fileos/path.h"

namespace f = fileos;

namespace unitos {
template<>
String toString(f::Path const& path)
{
    unitos::String result(path.length()*2);
    result << (char const*)path.data();
    result.terminate();
    return result;
}

template<>
String toString(containos::Utf8 const& str)
{
    unitos::String result(str.dataCount() + 1);
    result << (char const*)str.data();
    result.terminate();
    return result;
}
}

TEST_SUITE(Path)
{
    TEST(Invalid)
    {
        f::Path path;
        EXPECT_NULL(path.data());
    }

    TEST(ConvertSlashes)
    {
        f::Path path("folder\\test.foo");
        EXPECT_EQUAL(path, "folder/test.foo");
    }

    TEST(TrimFolders)
    {
        f::Path path("folder/.\\folder2/../test.foo");
        path.trimFolders();
        EXPECT_EQUAL(path, "folder/test.foo");
    }

    TEST(TrimFoldersComplex)
    {
        f::Path path("folder/.\\remove\\remove2/..\\./.././remove3\\../test.foo");
        path.trimFolders();
        EXPECT_EQUAL(path, "folder/test.foo");
    }

    TEST(Drive)
    {
        f::Path path("foobar:file.foo");
        EXPECT_EQUAL(path.drive(), "foobar");
        EXPECT_EQUAL(path.filename(), "file.foo");
    }

    TEST(Filename)
    {
        f::Path path("test:folder/test.foobar");
        EXPECT_EQUAL(path.filename(), "test.foobar");
    }

    TEST(Extension)
    {
        f::Path path("../test.foobar");
        EXPECT_EQUAL(path.extension(), "foobar");
    }

    TEST(NoExtension)
    {
        f::Path path("../test");
        EXPECT_EQUAL(path.extension(), "");
    }

    TEST(FolderHasDot)
    {
        f::Path path("../.tests/foo");
        EXPECT_EQUAL(path.extension(), "");
    }

    TEST(NoFixPath)
    {
        f::Path path("../../test");
        EXPECT_EQUAL(path, "../../test");
    }

    TEST(AutoFixPath)
    {
        f::Path path("/folder\\test.foobar\\");
        EXPECT_EQUAL(path, "folder/test.foobar");
    }

    TEST(Parent)
    {
        f::Path path("nakki/makkara/test.foobar");
        f::Path parent = path.parent();
        EXPECT_EQUAL(parent, "nakki/makkara");
    }
#if 0
    TEST(Catenate)
    {
        f::Path path;
        path.catenate("folder/folder2", "../file.foo");
        EXPECT_EQUAL(path.c_str(), "folder/file.foo");
    }

    TEST(CatenateWithSeparator)
    {
        f::Path path;
        path.catenate("folder/", "file.foo");
        EXPECT_EQUAL(path.c_str(), "folder/file.foo");
    }

    TEST(CatenateFirstEmpty)
    {
        f::Path path;
        path.catenate("", "folder/file.foo");
        EXPECT_EQUAL(path.c_str(), "folder/file.foo");
    }

    TEST(CatenateOnlySlashes)
    {
        f::Path path;
        path.catenate("/", "/folder/file.foo");
        EXPECT_EQUAL(path.c_str(), "folder/file.foo");
    }

    TEST(CatenateOnlySlashesFolder)
    {
        f::Path path;
        path.catenate("/folder/", "/");
        EXPECT_EQUAL(path.c_str(), "folder");
    }
#endif
}
