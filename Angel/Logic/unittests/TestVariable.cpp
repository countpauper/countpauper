#include "Logic/Knowledge.h"
#include "Logic/Variable.h"
#include "Logic/Expression.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{

TEST(Variable, Expression)
{
    EXPECT_FALSE(Variable(""));
    EXPECT_TRUE(Variable("Test"));
	EXPECT_EQ(Variable("Test"), Variable("Test"));
	EXPECT_NE(Variable("Test"), Variable("Icle"));
}

TEST(Variable, Query)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Variable("Test")), Variable("Test"));
	EXPECT_EQ(k.Infer(Variable("")), Variable(""));
	Variables vars{Equation{Variable("Test"), Integer(4)}};
	EXPECT_EQ(Variable("Test").Infer(k, vars), Integer(4));
}

TEST(Variable, Matching)
{
	EXPECT_EQ(Variable("X").Matches(Integer(1), {}), (Equation{Integer(1), Variable("X")}));
	EXPECT_EQ(Expression(Integer("2")).Matches(Variable("Y"), {}), (Equation{Variable("Y"), Integer(2)}));
    Variables substituted {Equation{Variable("Y"),Integer(2)}};
	EXPECT_EQ(Variable("Y").Matches(Integer(1), substituted), Boolean(false) );
	EXPECT_EQ(Variable("Y").Matches(Integer(2), substituted), Boolean(true) );
}

TEST(Variable, Substitute)
{
	EXPECT_EQ(Variable("B").Substitute(Conjunction{
			Equation{Variable("A"), Integer(3)},
			Equation{Variable("B"), Integer(2)}
		}),
		Integer(2));
	EXPECT_EQ(Variable("A").Substitute(Conjunction{Equation{Variable("B"), Integer(2)}}),
		Variable("A"));
}

TEST(Variable, MatchAnonymous)
{
	EXPECT_EQ(Variable("").Matches(Integer(1), {}), Boolean(true));
}

TEST(Variable, Hypothesis)
{
	Knowledge k{
		Predicate("cat", { Id("ginny")}) 
	};
	EXPECT_EQ(k.Infer(Predicate("cat", { Variable("C")})), (Association{
		Boolean(true),
		Equation{Variable("C"), Id("ginny")}
	}));
}

}
