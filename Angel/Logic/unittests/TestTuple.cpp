#include "Logic/Knowledge.h"
#include "Logic/All.h"
#include "Logic/Expression.h"
#include "Logic/Trace.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{

TEST(Tuple, Expression)
{
    EXPECT_FALSE(All(""));
    EXPECT_TRUE(All("Test"));
	EXPECT_EQ(All("Test"), All("Test"));
	EXPECT_NE(All("Test"), All("icle"));
}

TEST(Tuple, Query)
{
	Knowledge k;
	Trace trace;
	EXPECT_EQ(k.Infer(All("Test")), All("Test"));
	EXPECT_EQ(k.Infer(All("")), All(""));
	Hypothesis subs{Equal{Variable("Test"), List{Integer(2),Integer(3)}}};
	EXPECT_EQ(All("Test").Infer(k, subs, trace), (All(List{Integer(2),Integer(3)})));
}

TEST(Tuple, MatchingHypothesis)
{
	EXPECT_EQ((List{All("X")}).Matches(List{}, {}).Simplify(), (Equal{List{}, Variable("X")}));
	EXPECT_EQ((List{All("X")}).Matches(List{Integer(2)}, {}).Simplify(), (Equal{List{Integer(2)}, Variable("X")}));
	EXPECT_EQ((List{All("X")}).Matches(List{Integer(3),Integer(1)}, {}).Simplify(), (Equal{List{Integer(3),Integer(1)}, Variable("X")}));
	EXPECT_EQ((List{Integer(5), All("X")}).Matches(List{Integer(5),Integer(3)}, {}).Simplify(), (Equal{List{Integer(3),}, Variable("X")}));
}

TEST(Tuple, MatchingSubstitution)
{
	Hypothesis substituted {Equal{Variable("Y"),List{Integer(2), Integer(3)}}};
	EXPECT_EQ((List{Integer(1), Integer(3)}).Matches(List{All("Y")}, substituted).Simplify(), Boolean(false) );
	EXPECT_EQ((List{Integer(2), Integer(3)}).Matches(List{All("Y")}, substituted).Simplify(), Boolean(true) );
	EXPECT_EQ((List{Integer(1), Integer(2), Integer(3)}).Matches(List{Integer(1), All("Y")}, substituted).Simplify(), Boolean(true) );
}

TEST(Tuple, Substitute)
{
	EXPECT_THROW(All("B").Substitute(Conjunction{
			Equal{Variable("A"), Integer(3)},
			Equal{Variable("B"), Integer(2)}
		}), std::invalid_argument);
	EXPECT_EQ(All("V").Substitute(Conjunction{
			Equal{Variable("V"), List{Integer(2), Integer(4)}}}),
			All(List{Integer(2), Integer(4)}));

	EXPECT_EQ(All("A").Substitute(Conjunction{Equal{All("B"), Integer(2)}}),
		All("A"));
}

TEST(Tuple, MatchAnonymous)
{
	EXPECT_EQ((List{All("")}).Matches(List{Integer(1)}, {}).Simplify(), Boolean(true));
}

TEST(Tuple, Hypothesis)
{
	Knowledge k{
		Predicate("cat", { Id("ginny")}) 
	};
	EXPECT_EQ(k.Infer(Predicate("cat", { All("C")})), (Association{
		Boolean(true),
		Equal{Variable("C"), List{Id("ginny")}}
	}));
}

TEST(Tuple, InferValue)
{
	Knowledge k;
	Trace trace;
	Hypothesis subs {Equal{Variable("X"),List{Integer(24)}},
						Equal{Set{Integer(25)},Variable("Y")}};
	EXPECT_EQ(All("X").Infer(k, subs, trace), (All(List{Integer(24)})));	
	EXPECT_EQ(All("Y").Infer(k, subs, trace), (All(Set{Integer(25)})));	
}

TEST(Tuple, InferVariable)
{
	Knowledge k;
	Trace trace;
	Hypothesis subs {Equal{Variable("X"), (List{Integer(2)})}};
	EXPECT_EQ(All("Y").Infer(k, subs, trace), All("Y"));	
	EXPECT_EQ(All("").Infer(k, subs, trace), All(""));	
}

}
