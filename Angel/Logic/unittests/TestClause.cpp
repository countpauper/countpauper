#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Association.h"
#include "Logic/Conjunction.h"

namespace Angel::Logic::Test
{
	TEST(Clause, Trivial)
	{
		Knowledge k;
		k.Know(Predicate("cat"));
		EXPECT_EQ(Predicate("cat").Compute(k), Boolean(true));
		EXPECT_EQ(Predicate("dog").Compute(k), Boolean(false));
	}

	TEST(Clause, Condition)
	{
		Knowledge k;
		k.Know(Predicate("cat"), Predicate("ginny"));
		EXPECT_EQ(Predicate("cat").Compute(k), Boolean(false));
		k.Know(Predicate("ginny"));
		EXPECT_EQ(Predicate("cat").Compute(k), Boolean(true));
	}

	TEST(Clause, Conjunction)
	{
		Knowledge k;
		k.Know(Predicate("cat"), Conjunction{Predicate("fuzzy"), Predicate("noisy")});
		k.Know(Predicate("hamster"), Conjunction{Predicate("fuzzy"), Predicate("quiet")});
		EXPECT_EQ(k.Compute(Predicate("cat")), Boolean(false));
		EXPECT_EQ(k.Compute(Predicate("hamster")), Boolean(false));
		k.Know(Predicate("fuzzy"));
		k.Know(Predicate("noisy"));
		EXPECT_EQ(k.Compute(Predicate("cat")), Boolean(true));
		EXPECT_EQ(k.Compute(Predicate("hamster")), Boolean(false));
	}

	TEST(Clause, PredicateValence1)
	{
		Knowledge k;
		k.Know(Predicate("cat", List{Id("ginny")}),
            Predicate("fuzzy", List{Id("ginny")}));
		k.Know(Predicate("cat", List{Id("woofer")}),
            Predicate("fuzzy", List{Id("woofer")}));
		EXPECT_EQ(k.Compute(Predicate("cat", List{Id("ginny")})), Boolean(false));
		k.Know(Predicate("fuzzy", List{Id("ginny")}));

		EXPECT_EQ(Predicate("cat", List{Id("ginny")}).Compute(k), Boolean(true));
		EXPECT_EQ(Predicate("cat", List{Id("woofer")}).Compute(k), Boolean(false));
	}

	TEST(Clause, PredicateValence2)
	{
		Knowledge k;
		k.Know(Predicate("cats", List{Id("ginny"), Id("max")}),
				Conjunction{
            		Predicate("cat", List{Id("ginny")}),
					Predicate("cat", List{Id("max")}) });
		EXPECT_EQ(Predicate("cats", List{Id("ginny"), Id("max")}).Compute(k), Boolean(false));

		k.Know(Predicate("cat", List{Id("ginny")}));
		k.Know(Predicate("cat", List{Id("max")}));

		EXPECT_EQ(Predicate("cats", List{Id("ginny"), Id("max")}).Compute(k), Boolean(true));
	}


}
