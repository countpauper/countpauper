#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Association.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{
	TEST(Clause, Trivial)
	{
		Knowledge k{Predicate("cat")};
		EXPECT_EQ(k.Infer(Predicate("cat")), Boolean(true));
		EXPECT_EQ(k.Infer(Predicate("dog")), Boolean(false));
	}

	TEST(Clause, Condition)
	{
		Knowledge k{Association{Predicate("cat"), Predicate("ginny")}};
		EXPECT_EQ(k.Infer(Predicate("cat")), Boolean(false));
		k.Know(Predicate("ginny"));
		EXPECT_EQ(k.Infer(Predicate("cat")), Boolean(true));
	}

	TEST(Clause, Conjunction)
	{
		Knowledge k{ 
			Association{Predicate("cat"), Conjunction{Predicate("fuzzy"), Predicate("noisy")}},
			Association{Predicate("hamster"), Conjunction{Predicate("fuzzy"), Predicate("quiet")}}};
		EXPECT_EQ(k.Infer(Predicate("cat")), Boolean(false));
		EXPECT_EQ(k.Infer(Predicate("hamster")), Boolean(false));
		k.Know(Predicate("fuzzy"));
		k.Know(Predicate("noisy"));
		EXPECT_EQ(k.Infer(Predicate("cat")), Boolean(true));
		EXPECT_EQ(k.Infer(Predicate("hamster")), Boolean(false));
	}

	TEST(Clause, PredicateValence1)
	{
		Knowledge k{
			Association(Predicate("cat", List{Id("ginny")}),  Predicate("fuzzy", List{Id("ginny")})),
			Association(Predicate("cat", List{Id("woofer")}), Predicate("fuzzy", List{Id("woofer")}))};
		EXPECT_EQ(k.Infer(Predicate("cat", List{Id("ginny")})), Boolean(false));
		k.Know(Predicate("fuzzy", List{Id("ginny")}));

		EXPECT_EQ(k.Infer(Predicate("cat", List{Id("ginny")})), Boolean(true));
		EXPECT_EQ(k.Infer(Predicate("cat", List{Id("woofer")})), Boolean(false));
	}

	TEST(Clause, PredicateValence2)
	{
		Knowledge k{
			Association(Predicate("cats", List{Id("ginny"), Id("max")}),
				Conjunction{
            		Predicate("cat", List{Id("ginny")}),
					Predicate("cat", List{Id("max")}) })};
		EXPECT_EQ(k.Infer(Predicate("cats", List{Id("ginny"), Id("max")})), Boolean(false));

		k.Know(Predicate("cat", List{Id("ginny")}));
		k.Know(Predicate("cat", List{Id("max")}));

		EXPECT_EQ(k.Infer(Predicate("cats", List{Id("ginny"), Id("max")})), Boolean(true));
	}

	TEST(Clause, NumericConstant)
	{
		Knowledge k{Association(Predicate("c"), Integer(299'792'458))};
		EXPECT_EQ(k.Infer(Predicate("c")), Integer(299'792'458));
	}

	TEST(Clause, Arithmetic)
	{
		Knowledge k{Association(Predicate("plustwo", {Variable("X")}), Summation{Variable("X"), Integer(2)})};
		EXPECT_EQ(k.Infer(Predicate("plustwo", {Integer(-3)})), Integer(-1));
	}	
}
