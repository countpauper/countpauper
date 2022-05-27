#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Clause.h"
#include "Logic/Conjunction.h"

namespace Angel::Logic::Test
{

	TEST(TestClause, Trivial)
	{
		Knowledge k;
		k.Know(clause(Predicate("cat")));
		EXPECT_TRUE(k.Query(predicate("cat")).Trivial());
		EXPECT_FALSE(k.Query(predicate("dog")).Trivial());
	}

	TEST(TestClause, Condition)
	{
		Knowledge k;
		k.Know(clause(Predicate("cat"), predicate("ginny")));
		EXPECT_FALSE(k.Query(predicate("cat")).Trivial());
		k.Know(predicate("ginny"));
		EXPECT_TRUE(k.Query(predicate("cat")).Trivial());
	}

	TEST(TestClause, Conjunction)
	{
		Knowledge k;
		k.Know(clause(Predicate("cat"), conjunction(predicate("fuzzy"), predicate("noisy"))));
		k.Know(clause(Predicate("hamster"), conjunction(predicate("fuzzy"), predicate("quiet"))));
		EXPECT_FALSE(k.Query(predicate("cat")).Trivial());
		EXPECT_FALSE(k.Query(predicate("hamster")).Trivial());
		k.Know(predicate("fuzzy"));
		k.Know(predicate("noisy"));
		EXPECT_TRUE(k.Query(predicate("cat")).Trivial());
		EXPECT_FALSE(k.Query(predicate("hamster")).Trivial());
	}

	TEST(TestClause, Predicate1Ary)
	{
		Knowledge k;
		k.Know(clause(Predicate("cat", Sequence(id("ginny"))),
            predicate("fuzzy", Sequence(id("ginny")))));
		k.Know(clause(Predicate("cat", Sequence(id("woofer"))),
            predicate("fuzzy", Sequence(id("woofer")))));
		EXPECT_FALSE(k.Query(predicate("cat", Sequence(id("ginny")))).Trivial());
		k.Know(predicate("fuzzy", Sequence(id("ginny"))));

		EXPECT_TRUE(k.Query(predicate("cat", Sequence(id("ginny")))).Trivial());
		EXPECT_FALSE(k.Query(predicate("cat", Sequence(id("woofer")))).Trivial());
	}

	TEST(TestClause, Predicate2Ary)
	{
		Knowledge k;
		k.Know(clause(Predicate("cats", Sequence(id("ginny"), id("max"))),
            conjunction(predicate("cat", Sequence(id("ginny"))),
				predicate("cat", Sequence(id("max"))))));
		EXPECT_FALSE(k.Query(predicate("cats", Sequence(id("ginny"), id("max")))).Trivial());

		k.Know(predicate("cat", Sequence(id("ginny"))));
		k.Know(predicate("cat", Sequence(id("max"))));

		EXPECT_TRUE(k.Query(predicate("cats", Sequence(id("ginny"), id("max")))).Trivial());
	}


}
