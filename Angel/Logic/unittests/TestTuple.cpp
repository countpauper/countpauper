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
	Hypothesis subs{Equation{Variable("Test"), List{Integer(2),Integer(3)}}};
	EXPECT_EQ(Tuple("Test").Infer(k, subs), (List{Integer(2),Integer(3)}));
}

TEST(Tuple, MatchingHypothesis)
{
	EXPECT_EQ((List{Tuple("X")}).Matches(List{}, {}).Simplify(), (Equation{List{}, Variable("X")}));
	EXPECT_EQ((List{Tuple("X")}).Matches(List{Integer(2)}, {}).Simplify(), (Equation{List{Integer(2)}, Variable("X")}));
	EXPECT_EQ((List{Tuple("X")}).Matches(List{Integer(3),Integer(1)}, {}).Simplify(), (Equation{List{Integer(3),Integer(1)}, Variable("X")}));
	EXPECT_EQ((List{Integer(5), Tuple("X")}).Matches(List{Integer(5),Integer(3)}, {}).Simplify(), (Equation{List{Integer(3),}, Variable("X")}));
}

TEST(Tuple, MatchingSubstitution)
{
	Hypothesis substituted {Equation{Variable("Y"),List{Integer(2), Integer(3)}}};
	EXPECT_EQ((List{Integer(1), Integer(3)}).Matches(List{Tuple("Y")}, substituted).Simplify(), Boolean(false) );
	EXPECT_EQ((List{Integer(2), Integer(3)}).Matches(List{Tuple("Y")}, substituted).Simplify(), Boolean(true) );
	EXPECT_EQ((List{Integer(1), Integer(2), Integer(3)}).Matches(List{Integer(1), Tuple("Y")}, substituted).Simplify(), Boolean(true) );
}

TEST(Tuple, Substitute)
{
	EXPECT_EQ(Tuple("B").Substitute(Conjunction{
			Equation{Variable("A"), Integer(3)},
			Equation{Variable("B"), Integer(2)}
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
		Equation{Variable("C"), List{Id("ginny")}}
	}));
}



TEST(Tuple, InferValue)
{
	Knowledge k;
	Hypothesis subs {Equation{Variable("X"),List{Integer(24)}},
						Equation{Set{Integer(25)},Variable("Y")}};
	EXPECT_EQ(Tuple("X").Infer(k, subs), (List{Integer(24)}));	
	EXPECT_EQ(Tuple("Y").Infer(k, subs), (Set{Integer(25)}));	
}

TEST(Tuple, InferVariable)
{
	Knowledge k;
	Hypothesis subs {Equation{Variable("X"), (List{Integer(2)})}};
	EXPECT_EQ(Tuple("Y").Infer(k, subs), Tuple("Y"));	
	EXPECT_EQ(Tuple("").Infer(k, subs), Tuple(""));	
}

}
