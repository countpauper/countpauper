#include "Logic/Knowledge.h"
#include "Logic/Tuple.h"
#include "Logic/Expression.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{

TEST(Tuple, Expression)
{
    EXPECT_FALSE(Tuple(""));
    EXPECT_TRUE(Tuple("Test"));
	EXPECT_EQ(Tuple("Test"), Tuple("Test"));
	EXPECT_NE(Tuple("Test"), Tuple("icle"));
}

TEST(Tuple, Query)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Tuple("Test")), Tuple("Test"));
	EXPECT_EQ(k.Infer(Tuple("")), Tuple(""));
	Substitutions subs{Equation{Tuple("Test"), List{Integer(2),Integer(3)}}};
	EXPECT_EQ(Tuple("Test").Infer(k, subs), (List{Integer(2),Integer(3)}));
}

TEST(Tuple, MatchingHypothesis)
{
	EXPECT_EQ((List{Tuple("X")}).Matches(List{}, {}).Simplify(), (Equation{List{}, Tuple("X")}));
	EXPECT_EQ((List{Tuple("X")}).Matches(List{Integer(2)}, {}).Simplify(), (Equation{List{Integer(2)}, Tuple("X")}));
	EXPECT_EQ((List{Tuple("X")}).Matches(List{Integer(3),Integer(1)}, {}).Simplify(), (Equation{List{Integer(3),Integer(1)}, Tuple("X")}));
	EXPECT_EQ((List{Integer(5), Tuple("X")}).Matches(List{Integer(5),Integer(3)}, {}).Simplify(), (Equation{List{Integer(3),}, Tuple("X")}));
}

TEST(Tuple, MatchingSubstitution)
{
	Substitutions substituted {Equation{Tuple("Y"),List{Integer(2), Integer(3)}}};
	EXPECT_EQ((List{Integer(1), Integer(3)}).Matches(List{Tuple("Y")}, substituted).Simplify(), Boolean(false) );
	EXPECT_EQ((List{Integer(2), Integer(3)}).Matches(List{Tuple("Y")}, substituted).Simplify(), Boolean(true) );
	EXPECT_EQ((List{Integer(1), Integer(2), Integer(3)}).Matches(List{Integer(1), Tuple("Y")}, substituted).Simplify(), Boolean(true) );
}

TEST(Tuple, Substitute)
{
	EXPECT_EQ(Tuple("B").Substitute(Conjunction{
			Equation{Tuple("A"), Integer(3)},
			Equation{Tuple("B"), Integer(2)}
		}),
		Integer(2));
	EXPECT_EQ(Tuple("A").Substitute(Conjunction{Equation{Tuple("B"), Integer(2)}}),
		Tuple("A"));
}

TEST(Tuple, MatchAnonymous)
{
	EXPECT_EQ((List{Tuple("")}).Matches(List{Integer(1)}, {}).Simplify(), Boolean(true));
}

TEST(Tuple, Hypothesis)
{
	Knowledge k{
		Predicate("cat", { Id("ginny")}) 
	};
	EXPECT_EQ(k.Infer(Predicate("cat", { Tuple("C")})), (Association{
		Boolean(true),
		Equation{Tuple("C"), List{Id("ginny")}}
	}));
}

}
