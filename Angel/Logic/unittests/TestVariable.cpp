#include "Logic/Knowledge.h"
#include "Logic/Variable.h"
#include "Logic/Expression.h"
#include "Logic/Trace.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{

TEST(Variable, Construction)
{
    EXPECT_FALSE(Variable(""));
    EXPECT_TRUE(Variable("Test"));
	EXPECT_EQ(Variable("Test"), Variable("Test"));
	EXPECT_NE(Variable("Test"), Variable("Icle"));
    EXPECT_EQ(Expression(PrefixOperator(L'$'), {Id("Test")}), Variable("Test"));
}

TEST(Variable, Query)
{
	Knowledge k;
	Trace trace;
	EXPECT_EQ(k.Infer(Variable("Test")), Variable("Test"));
	EXPECT_EQ(k.Infer(Variable("")), Variable(""));
	Hypothesis subs{Equal{Variable("Test"), Integer(4)}};
	EXPECT_EQ(Variable("Test").Infer(k, subs, trace), Integer(4));
}

TEST(Variable, MatchingHypothesis)
{
	EXPECT_EQ(Variable("X").Matches(Integer(1), {}), (Equal{Integer(1), Variable("X")}));
	EXPECT_EQ(Expression(Integer("2")).Matches(Variable("Y"), {}), (Equal{Variable("Y"), Integer(2)}));
}

TEST(Variable, MatchingSubstitution)
{
	Hypothesis substituted {Equal{Variable("Y"),Integer(2)}};
	EXPECT_EQ(Variable("Y").Matches(Integer(1), substituted), Boolean(false) );
	EXPECT_EQ(Variable("Y").Matches(Integer(2), substituted), Boolean(true) );
}

TEST(Variable, Substitute)
{
	EXPECT_EQ(Variable("B").Substitute(Conjunction{
			Equal{Variable("A"), Integer(3)},
			Equal{Variable("B"), Integer(2)}
		}),
		Integer(2));

	EXPECT_EQ(Variable("T").Substitute(Conjunction{
			Equal{Variable("T"), List{Integer(-1), Integer(3)}}}),
			(List{Integer(-1), Integer(3)}));
	
	EXPECT_EQ(Variable("A").Substitute(Conjunction{Equal{Variable("B"), Integer(2)}}),
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
		Equal{Variable("C"), Id("ginny")}
	}));
}

TEST(Variable, InferValue)
{
	Knowledge k;
	Trace trace;
	Hypothesis subs {Equal{Variable("X"),Integer(24)},
						Equal{Integer(25), Variable("Y")}};
	EXPECT_EQ(Variable("X").Infer(k, subs, trace), Integer(24));	
	EXPECT_EQ(Variable("Y").Infer(k, subs, trace), Integer(25));	
}

TEST(Variable, InferVariable)
{
	Knowledge k;
	Trace trace;
	Hypothesis subs {Equal{Variable("X"),Integer(2)}};
	EXPECT_EQ(Variable("Y").Infer(k, subs, trace), Variable("Y"));	
	EXPECT_EQ(Variable("").Infer(k, subs, trace), Variable(""));	
}

}