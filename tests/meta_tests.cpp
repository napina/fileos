// Copyright (C) Ville Ruusutie, 2011

#include "unitos/unitos.h"
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include "fileos/meta.h"
#include "fileos/bufferin.h"

namespace fileos {

template<>
int parse<int>(char const* str)
{
    return ::atoi(str);
}

} // end of fileos
namespace f = fileos;

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

        f::MetaNode const* node = parser.find("Test");
        EXPECT_VALID(parser.find("Test"));
        EXPECT_EQUAL(node->typeName(), "int");
        EXPECT_EQUAL(node->value(), "133");
        EXPECT_EQUAL(node->valueAs<int>(), 133);
    }

    TEST_FIXTURE(MetaFixture, ParseEmptyList)
    {
        unitos::String str("#list Test [ \n  \t]");

        f::MetaParser parser;
        bool ok = parser.parse(str.c_str(), str.length());
        EXPECT_TRUE(ok);

        f::MetaNode const* listNode = parser.find("Test");
        EXPECT_VALID(parser.find("Test"));
        EXPECT_EQUAL(listNode->typeName(), "list");
        EXPECT_NULL(listNode->firstChild());
    }

    TEST_FIXTURE(MetaFixture, ParseList)
    {
        unitos::String str("#list Test [int \tFirst [6] int Second \n[32]]");

        f::MetaParser parser;
        bool ok = parser.parse(str.c_str(), str.length());
        EXPECT_TRUE(ok);

        f::MetaNode const* listNode = parser.find("Test");
        EXPECT_VALID(listNode);
        EXPECT_VALID(listNode->findChild("First"));
        EXPECT_VALID(listNode->findChild("Second"));
        EXPECT_EQUAL(listNode->typeName(), "list");

        f::MetaNode const* firstNode = listNode->findChild("First");
        f::MetaNode const* secondNode = listNode->findChild("Second");
        EXPECT_EQUAL(firstNode->typeName(), "int");
        EXPECT_EQUAL(firstNode->value(), "6");
        EXPECT_EQUAL(firstNode->valueAs<int>(), 6);
        EXPECT_EQUAL(secondNode->typeName(), "int");
        EXPECT_EQUAL(secondNode->value(), "32");
        EXPECT_EQUAL(secondNode->valueAs<int>(), 32);
    }

    TEST_FIXTURE(MetaFixture, ParseStream)
    {
        f::BufferIn buffer("#list Test [int\t Value [6]]");

        f::MetaParser parser;
        bool ok = parser.parse(buffer);
        EXPECT_TRUE(ok);

        f::MetaNode const* listNode = parser.find("Test");
        EXPECT_VALID(listNode);
        EXPECT_VALID(listNode->findChild("Value"));
        EXPECT_EQUAL(listNode->typeName(), "list");

        f::MetaNode const* valueNode = listNode->findChild("Value");
        EXPECT_EQUAL(valueNode->typeName(), "int");
        EXPECT_EQUAL(valueNode->value(), "6");
        EXPECT_EQUAL(valueNode->valueAs<int>(), 6);
    }

    TEST_FIXTURE(MetaFixture, ParseTwoList)
    {
        unitos::String str("#list Test1 [int Value [3]] #list Test2 [int Value [6]]");

        f::MetaParser parser;
        bool ok = parser.parse(str.c_str(), str.length());
        EXPECT_TRUE(ok);
        EXPECT_VALID(parser.find("Test1"));
        EXPECT_VALID(parser.find("Test2"));

        f::MetaNode const* list1Node = parser.find("Test1");
        f::MetaNode const* list2Node = parser.find("Test2");
        EXPECT_EQUAL(parser.first(), list1Node);
        EXPECT_EQUAL(list1Node->next(), list2Node);

        EXPECT_EQUAL(list1Node->typeName(), "list");
        EXPECT_VALID(list1Node->findChild("Value"));
        EXPECT_EQUAL(list2Node->typeName(), "list");
        EXPECT_VALID(list2Node->findChild("Value"));

        f::MetaNode const* firstNode = list1Node->findChild("Value");
        f::MetaNode const* secondNode = list2Node->findChild("Value");
        EXPECT_EQUAL(firstNode->typeName(), "int");
        EXPECT_EQUAL(firstNode->value(), "3");
        EXPECT_EQUAL(firstNode->valueAs<int>(), 3);
        EXPECT_EQUAL(secondNode->typeName(), "int");
        EXPECT_EQUAL(secondNode->value(), "6");
        EXPECT_EQUAL(secondNode->valueAs<int>(), 6);
    }
}
