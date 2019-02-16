#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"

#pragma warning(disable:4566)	// google test can't represent unicode

namespace Angel
{
namespace Parser
{
namespace Test
{

TEST(TestParser, Id)
{
	Logic::Knowledge k = Parse(L"cat");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::id(L"cat")));
	EXPECT_FALSE(k.Knows(Logic::id(L"dog")));
}

TEST(TestParser, Ids)
{
	Logic::Knowledge k = Parse(L"max ginny");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::id(L"max")));
	EXPECT_TRUE(k.Knows(Logic::id(L"ginny")));
}

TEST(TestParser, Boolean)
{
	Logic::Knowledge k = Parse(L"true false");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::boolean(true)));
	EXPECT_TRUE(k.Knows(Logic::boolean(false)));
}

TEST(TestParser, Predicate)
{
	Logic::Knowledge k = Parse(L"cat(ginny) ginny");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"cat", Logic::sequence())));
	EXPECT_TRUE(k.Knows(Logic::id(L"ginny")));
	EXPECT_FALSE(k.Knows(Logic::predicate(L"dog", Logic::sequence())));
}

TEST(TestParser, Whitespace)
{
	Logic::Knowledge k = Parse(L"\t  cat(\n )\r\n");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"cat", Logic::sequence())));
}

TEST(TestParser, UnicodeId)
{
	Logic::Knowledge k = Parse(L"Γάτα() 猫");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"Γάτα", Logic::sequence())));
	EXPECT_TRUE(k.Knows(Logic::id(L"猫")));
}

}
}
}

