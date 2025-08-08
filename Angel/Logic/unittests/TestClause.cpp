#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Logic/Association.h"
#include "Logic/Expression.h"
#include "Logic/Summation.h"

namespace Angel::Logic::Test
{
	TEST(Clause, Trivial)
	{
		Knowledge k{Predicate("cat")};
		EXPECT_EQ(k.Infer(Predicate("cat")), True);
		EXPECT_EQ(k.Infer(Predicate("dog")), False);
	}

	TEST(Clause, Condition)
	{
		Knowledge k{Association{Predicate("cat"), Predicate("ginny")}};
		EXPECT_EQ(k.Infer(Predicate("cat")), False);
		k.Know(Predicate("ginny"));
		EXPECT_EQ(k.Infer(Predicate("cat")), True);
	}

	TEST(Clause, Conjunction)
	{
		Knowledge k{ 
			Association{Predicate("cat"), Conjunction{Predicate("fuzzy"), Predicate("noisy")}},
			Association{Predicate("hamster"), Conjunction{Predicate("fuzzy"), Predicate("quiet")}}};
		EXPECT_EQ(k.Infer(Predicate("cat")), False);
		EXPECT_EQ(k.Infer(Predicate("hamster")), False);
		k.Know(Predicate("fuzzy"));
		k.Know(Predicate("noisy"));
		EXPECT_EQ(k.Infer(Predicate("cat")), True);
		EXPECT_EQ(k.Infer(Predicate("hamster")), False);
	}

	TEST(Clause, PredicateValence1)
	{
		Knowledge k{
			Association(Predicate("cat", List{Id("ginny")}),  Predicate("fuzzy", List{Id("ginny")})),
			Association(Predicate("cat", List{Id("woofer")}), Predicate("fuzzy", List{Id("woofer")}))};
		EXPECT_EQ(k.Infer(Predicate("cat", List{Id("ginny")})), False);
		k.Know(Predicate("fuzzy", List{Id("ginny")}));

		EXPECT_EQ(k.Infer(Predicate("cat", List{Id("ginny")})), True);
		EXPECT_EQ(k.Infer(Predicate("cat", List{Id("woofer")})), False);
	}

	TEST(Clause, PredicateValence2)
	{
		Knowledge k{
			Association(Predicate("cats", List{Id("ginny"), Id("max")}),
				Conjunction{
            		Predicate("cat", List{Id("ginny")}),
					Predicate("cat", List{Id("max")}) })};
		EXPECT_EQ(k.Infer(Predicate("cats", List{Id("ginny"), Id("max")})), False);

		k.Know(Predicate("cat", List{Id("ginny")}));
		k.Know(Predicate("cat", List{Id("max")}));

		EXPECT_EQ(k.Infer(Predicate("cats", List{Id("ginny"), Id("max")})), True);
	}

	TEST(Clause, NumericConstant)
	{
		Knowledge k{Association(Predicate("c"), Integer(299'792'458))};
		EXPECT_EQ(k.Infer(Predicate("c")), Integer(299'792'458));
	}

	TEST(Clause, Arithmetic)
	{
		Knowledge k{Association(Predicate("plustwo", {Variable("X")}), Summation({Variable("X"), Integer(2)}))};
		EXPECT_EQ(k.Infer(Predicate("plustwo", {Integer(-3)})), Integer(-1));
	}	
}
