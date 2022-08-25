#include "pch.h"
#include "Parser/LogicParser.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"

#pragma warning(disable:4566)	// google test can't represent unicode

namespace Angel::Parser::Test
{

TEST(TestParser, Empty)
{
	Logic::Knowledge k = Parse("");
	EXPECT_EQ(k.Clauses(), 0);

	Logic::Knowledge w = Parse("  \t   \r\n  ");
	EXPECT_EQ(w.Clauses(), 0);
}

TEST(TestParser, Whitespace)
{
	Logic::Knowledge k = Parse("\t  cat(\n )\r\n");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::predicate("cat")));
}

TEST(TestParser, UnicodeId)
{   // TODO: unicode sort of works now but the predicate is parsed as an id and the id an empty () expression predicate() should have 
    std::stringstream s("Γάτα() 猫");

    Logic::Object predicate, id;
    s >> predicate >> id;   
	EXPECT_EQ(predicate, Logic::predicate("Γάτα"));
    EXPECT_EQ(id, Logic::id("猫"));
}

}

