#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Set.h"

namespace Angel
{
namespace Parser
{
namespace Test
{

TEST(TestSet, Empty)
{
	Logic::Knowledge k = Parse(L"{} ");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::set()));
}


TEST(TestSet, Single)
{
	Logic::Knowledge k = Parse(L"{ cat}");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::set(Logic::id(L"cat"))));
	EXPECT_FALSE(k.Knows(Logic::id(L"cat")));
}


TEST(TestSet, Multiple)
{
	Logic::Knowledge k = Parse(L"{ cat, dog, cat } ");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::set(Logic::id(L"cat"), Logic::id(L"dog"))));
	EXPECT_FALSE(k.Knows(Logic::id(L"cat")));
	EXPECT_FALSE(k.Knows(Logic::id(L"dog")));
}

TEST(TestSet, Nested)
{
	Logic::Knowledge k = Parse(L"{ {cat, dog }, cat}");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::set(
		Logic::set(Logic::id(L"cat"), Logic::id(L"dog")), 
		Logic::id(L"cat"))));
	EXPECT_FALSE(k.Knows(Logic::set(Logic::id(L"cat"),
		Logic::id(L"dog"))));
}


}
}
}

