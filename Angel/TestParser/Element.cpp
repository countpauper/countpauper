#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"

namespace Angel
{
namespace Parser
{
namespace Test
{

TEST(TestElement, Id)
{
	Logic::Knowledge k = Parse(L"cat");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::id(L"cat")));
	EXPECT_FALSE(k.Knows(Logic::id(L"dog")));
}


TEST(TestElement, Ids)
{
	Logic::Knowledge k = Parse(L"max ginny");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::id(L"max")));
	EXPECT_TRUE(k.Knows(Logic::id(L"ginny")));
}

TEST(TestElement, Boolean)
{
	Logic::Knowledge k = Parse(L"true false");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::boolean(true)));
	EXPECT_TRUE(k.Knows(Logic::boolean(false)));
}

TEST(TestElement, Integer)
{
	Logic::Knowledge k = Parse(L"-9 23 cat2");

	EXPECT_EQ(k.Clauses(), 3);
	EXPECT_TRUE(k.Knows(Logic::integer(23)));
	EXPECT_TRUE(k.Knows(Logic::integer(-9)));
	EXPECT_TRUE(k.Knows(Logic::id(L"cat2")));
}



}
}
}

