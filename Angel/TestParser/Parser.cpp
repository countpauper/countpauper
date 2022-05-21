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

TEST(TestParser, Empty)
{
	Logic::Knowledge k = Parse(L"");
	EXPECT_EQ(k.Clauses(), 0);

	Logic::Knowledge w = Parse(L"  \t   \r\n  ");
	EXPECT_EQ(w.Clauses(), 0);
}


TEST(TestParser, Whitespace)
{
	Logic::Knowledge k = Parse(L"\t  cat(\n )\r\n");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"cat")));
}

TEST(TestParser, UnicodeId)
{
	Logic::Knowledge k = Parse(L"Γάτα() 猫");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"Γάτα")));
	EXPECT_TRUE(k.Knows(Logic::predicate(L"猫")));
}

}
}
}

