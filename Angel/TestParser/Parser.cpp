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

