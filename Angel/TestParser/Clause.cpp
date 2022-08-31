#include "pch.h"
#include "Parser/LogicParser.h"
#include "Logic/Clause.h"
#include "Logic/Conjunction.h"
#include "Logic/Knowledge.h"

namespace Angel::Parser::Test
{



TEST(TestClause, Predicate)
{
	Logic::Knowledge k = Parse("cat(ginny) ginny()");

    // TODO doesn't work because (ginny) is not a braced sequence, because ginny is not a comma sequence, because it's just one ginny. 
    // In Sequence tests this is accepted because braced expression creates a sequence in LogicParser Why doesn't loop accept zero loops ? 
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

/*  // reeanble with BNF
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

