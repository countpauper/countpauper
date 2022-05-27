#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Clause.h"
#include "Logic/Conjunction.h"

namespace Angel::Parser::Test
{

/*  // reeanble with BNF

TEST(TestClause, Predicate)
{
	Logic::Knowledge k = Parse("cat(ginny) ginny()");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::predicate("cat", Logic::Sequence(Logic::id("ginny")))));
	EXPECT_TRUE(k.Knows(Logic::predicate("ginny")));
	EXPECT_FALSE(k.Knows(Logic::predicate("dog")));
}

TEST(TestClause, Condition)
{
	Logic::Knowledge k = Parse("cat() : ginny() ginny()");

	EXPECT_EQ(k.Clauses(), 2);
	EXPECT_TRUE(k.Knows(Logic::clause(Logic::Predicate("cat"), Logic::predicate("ginny"))));
	EXPECT_FALSE(k.Knows(Logic::clause(Logic::Predicate("cat"))));
	EXPECT_TRUE(k.Knows(Logic::predicate("ginny")));
}

TEST(TestClause, Conditions)
{
	Logic::Knowledge k = Parse("cat() : fuzzy(), noisy()");

	EXPECT_EQ(k.Clauses(), 1);
	EXPECT_TRUE(k.Knows(Logic::clause(Logic::Predicate("cat"), 
						Logic::conjunction(Logic::predicate("fuzzy"),
										Logic::predicate("noisy")))));
}
*/

}

