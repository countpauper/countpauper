#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Clause.h"

#pragma warning(disable:4566)	// google test can't represent unicode

namespace Angel
{
namespace Parser
{
namespace Test
{

TEST(TestClause, Predicate)
{
	Logic::Knowledge k = Parse(L"cat(ginny) ginny()");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::predicate(L"cat", Logic::Sequence(Logic::id(L"ginny")))));
	EXPECT_TRUE(k.Knows(Logic::predicate(L"ginny")));
	EXPECT_FALSE(k.Knows(Logic::predicate(L"dog")));
}

TEST(TestClause, Condition)
{
	Logic::Knowledge k = Parse(L"cat() : ginny() ginny()");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::clause(Logic::Predicate(L"cat"), Logic::Sequence(Logic::predicate(L"ginny")))));
	EXPECT_FALSE(k.Knows(Logic::clause(Logic::Predicate(L"cat"))));
	EXPECT_TRUE(k.Knows(Logic::predicate(L"ginny")));
}

TEST(TestClause, Conditions)
{
	Logic::Knowledge k = Parse(L"cat() : fuzzy(), noisy()");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::clause(Logic::Predicate(L"cat"), 
						Logic::Sequence(Logic::predicate(L"fuzzy"),
										Logic::predicate(L"noisy")))));
}

}
}
}

