#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Sequence.h"
#include "Logic/Array.h"

#pragma warning(disable:4566)	// google test can't represent unicode

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

TEST(TestSequence, Commas)
{
	Logic::Knowledge k = Parse(L"cat, dog, hamster");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::array(
		Logic::id(L"cat"), 
		Logic::id(L"dog"),
		Logic::id(L"hamster"))));
}

TEST(TestSequence, Braces)
{
	Logic::Knowledge k = Parse(L"( cat)");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::sequence(Logic::id(L"cat"))));
	EXPECT_FALSE(k.Knows(Logic::id(L"cat")));
}


TEST(TestSequence, BracesAndComma)
{
	Logic::Knowledge k = Parse(L"( cat, dog )");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::sequence(Logic::id(L"cat"), Logic::id(L"dog"))));
	EXPECT_FALSE(k.Knows(Logic::id(L"cat")));
	EXPECT_FALSE(k.Knows(Logic::id(L"dog")));
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

