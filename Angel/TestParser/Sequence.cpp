#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Sequence.h"
#include "Logic/Array.h"

namespace Angel
{
namespace Parser
{
namespace Test
{

TEST(TestSequence, Comma)
{
	Logic::Knowledge k = Parse(L"cat, dog");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::array(Logic::id(L"cat"), Logic::id(L"dog"))));
	EXPECT_FALSE(k.Knows(Logic::id(L"cat")));
	EXPECT_FALSE(k.Knows(Logic::id(L"dog")));
}

TEST(TestSequence, MakeArray)
{
	Logic::Knowledge k = Parse(L"cat, ");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::array(Logic::id(L"cat"))));
	EXPECT_FALSE(k.Knows(Logic::id(L"cat")));
}

TEST(TestSequence, Commas)
{
	Logic::Knowledge k = Parse(L"cat, dog, hamster");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::array(
		Logic::id(L"cat"), 
		Logic::id(L"dog"),
		Logic::id(L"hamster"))));
}


TEST(TestSequence, Empty)
{
	Logic::Knowledge k = Parse(L"( )");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::sequence()));
}

TEST(TestSequence, Single)
{
	Logic::Knowledge k = Parse(L"( cat)");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::sequence(Logic::id(L"cat"))));
	EXPECT_FALSE(k.Knows(Logic::id(L"cat")));
}


TEST(TestSequence, Multiple)
{
	Logic::Knowledge k = Parse(L"( cat, dog )");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::sequence(Logic::id(L"cat"), Logic::id(L"dog"))));
	EXPECT_FALSE(k.Knows(Logic::id(L"cat")));
	EXPECT_FALSE(k.Knows(Logic::id(L"dog")));
}

TEST(TestSequence, DISABLED_Errors)
{
	EXPECT_THROW(Parse(L"cat, dog )"), std::runtime_error);
	EXPECT_THROW(Parse(L"(cat, dog"), std::runtime_error);
	EXPECT_THROW(Parse(L"(cat, dog}"), std::runtime_error);
}

TEST(TestSequence, Nested)
{
	Logic::Knowledge k = Parse(L"( cat, (dog, hamster) )");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::sequence(Logic::id(L"cat"), Logic::sequence(
		Logic::id(L"dog"), Logic::id(L"hamster")))));
	EXPECT_FALSE(k.Knows(Logic::sequence(Logic::id(L"cat"), 
		Logic::id(L"dog"), 
		Logic::id(L"hamster"))));
}


}
}
}

