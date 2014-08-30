// Copyright (C) Ville Ruusutie, 2011

#include "unitos/unitos.h"
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include "fileos/meta.h"
#include "fileos/bufferin.h"
#include "containos/utf8.h"

namespace fileos {

template<>
int parse<int>(uint8_t const* str)
{
    return ::atoi((const char*)str);
}

} // end of fileos
namespace f = fileos;
namespace c = containos;

class MetaFixture : public unitos::SuiteTest
{
    /*typedef rapidos::Types<
        rapidos::Type<int,"int">,
        rapidos::Type<float,"float"> > ValueTypes;*/
public:
    virtual ~MetaFixture() {}
    MetaFixture()
    {
    }

protected:
};

TEST_SUITE(Meta)
{
    TEST_FIXTURE(MetaFixture, ParseInt)
    {
        unitos::String str("int Test [133]");

        f::MetaParser parser;
        bool ok = parser.parse(str.c_str(), str.length());
        EXPECT_TRUE(ok);

        f::MetaNode const* node = parser.findChild("Test");
        EXPECT_VALID(parser.findChild("Test"));
        EXPECT_EQUAL(node->typeName(), "int");
        EXPECT_EQUAL(node->valueAs<int>(), 133);
    }

    TEST_FIXTURE(MetaFixture, ParseEmptyList)
    {
        unitos::String str("#list Test [ \n  \t]");

        f::MetaParser parser;
        bool ok = parser.parse(str.c_str(), str.length());
        EXPECT_TRUE(ok);

        f::MetaNode const* listNode = parser.findChild("Test");
        EXPECT_VALID(parser.findChild("Test"));
        EXPECT_EQUAL(listNode->typeName(), "list");
        EXPECT_NULL(listNode->firstChild());
    }

    TEST_FIXTURE(MetaFixture, ParseList)
    {
        unitos::String str("#list Test [int \tFirst [6] int Second \n[32]]");

        f::MetaParser parser;
        bool ok = parser.parse(str.c_str(), str.length());
        EXPECT_TRUE(ok);

        f::MetaNode const* listNode = parser.findChild("Test");
        EXPECT_VALID(listNode);
        EXPECT_VALID(listNode->findChild("First"));
        EXPECT_VALID(listNode->findChild("Second"));
        EXPECT_EQUAL(listNode->typeName(), "list");

        f::MetaNode const* firstNode = listNode->findChild("First");
        f::MetaNode const* secondNode = listNode->findChild("Second");
        EXPECT_EQUAL(firstNode->typeName(), "int");
        EXPECT_EQUAL(firstNode->valueAs<int>(), 6);
        EXPECT_EQUAL(secondNode->typeName(), "int");
        EXPECT_EQUAL(secondNode->valueAs<int>(), 32);
    }

    TEST_FIXTURE(MetaFixture, ParseStream)
    {
        f::BufferIn buffer("#list Test [int\t Value [6]]");

        f::MetaParser parser;
        bool ok = parser.parse(buffer);
        EXPECT_TRUE(ok);

        f::MetaNode const* listNode = parser.findChild("Test");
        EXPECT_VALID(listNode);
        EXPECT_VALID(listNode->findChild("Value"));
        EXPECT_EQUAL(listNode->typeName(), "list");

        f::MetaNode const* valueNode = listNode->findChild("Value");
        EXPECT_EQUAL(valueNode->typeName(), "int");
        EXPECT_EQUAL(valueNode->valueAs<int>(), 6);
    }

    TEST_FIXTURE(MetaFixture, ParseTwoList)
    {
        unitos::String str("#list Test1 [int Value [3]] #list Test2 [int Value [6]]");

        f::MetaParser parser;
        bool ok = parser.parse(str.c_str(), str.length());
        EXPECT_TRUE(ok);
        EXPECT_VALID(parser.findChild("Test1"));
        EXPECT_VALID(parser.findChild("Test2"));

        f::MetaNode const* list1Node = parser.findChild("Test1");
        f::MetaNode const* list2Node = parser.findChild("Test2");
        EXPECT_EQUAL(parser.firstChild(), list1Node);
        EXPECT_EQUAL(list1Node->next(), list2Node);

        EXPECT_EQUAL(list1Node->typeName(), "list");
        EXPECT_VALID(list1Node->findChild("Value"));
        EXPECT_EQUAL(list2Node->typeName(), "list");
        EXPECT_VALID(list2Node->findChild("Value"));

        f::MetaNode const* firstNode = list1Node->findChild("Value");
        f::MetaNode const* secondNode = list2Node->findChild("Value");
        EXPECT_EQUAL(firstNode->typeName(), "int");
        EXPECT_EQUAL(firstNode->valueAs<int>(), 3);
        EXPECT_EQUAL(secondNode->typeName(), "int");
        EXPECT_EQUAL(secondNode->valueAs<int>(), 6);
    }
}
