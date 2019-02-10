#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"

#pragma warning(disable:4566)	// google test can't represent unicode

namespace Angel
{
namespace Parser
{
namespace Test
{

TEST(TestParser, Id)
{
	Logic::Knowledge k = Parse(L"test");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::id(L"test")));
	EXPECT_FALSE(k.Knows(Logic::id(L"not_a_test")));
}

TEST(TestParser, Predicate)
{
	Logic::Knowledge k = Parse(L"test()");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"test", Logic::sequence())));
	EXPECT_FALSE(k.Knows(Logic::predicate(L"not_a_test", Logic::sequence())));
}

TEST(TestParser, Whitespace)
{
	Logic::Knowledge k = Parse(L"\t  test (\n )\r\n");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"test", Logic::sequence())));
}

TEST(TestParser, UnicodeId)
{
	Logic::Knowledge k = Parse(L"βçݝﬣ鳳()");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"βçݝﬣ鳳", Logic::sequence())));
} 

}
}
}

