#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/All.h"
#include "Logic/Expression.h"
#include "Logic/Trace.h"

namespace Angel::Logic::Test
{

TEST(All, Construction)
{
    EXPECT_TRUE(bool(All(List{})));
    EXPECT_EQ(All(List{Integer(3)}), (All{Integer(3)}));
    EXPECT_NE(All(List{Integer(3)}), All(List{Integer(-3)}));
    EXPECT_EQ(Expression(PrefixOperator(L'∀'), Collection{List{Integer(-2)}}), All(List{Integer(-2)})); 
    EXPECT_EQ(Expression(PrefixOperator(L'∀'), Collection{Id("T")}), All("T"));
}

TEST(All, ConstructValliable)
{
    EXPECT_FALSE(All(""));
    EXPECT_TRUE(All("Test"));
	EXPECT_EQ(All("Test"), All("Test"));
	EXPECT_NE(All("Test"), All("icle"));
}

TEST(All, Simplify)
{
    EXPECT_EQ(All(List{Summation{Integer(2)}}).Simplify(), 
        All(List{Integer(2)}));
}

TEST(All, Substitute)
{
	EXPECT_EQ(All("P").Substitute(Conjunction{Equal{Variable("P"), List{Integer(3)}}}),
		All(List{Integer(3)}));
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

TEST(All, Query)
{
	Knowledge k;
	Trace trace;
	EXPECT_EQ(k.Infer(All("Test")), All("Test"));
	EXPECT_EQ(k.Infer(All("")), All(""));
	Hypothesis subs{Equal{Variable("Test"), List{Integer(2),Integer(3)}}};
	EXPECT_EQ(All("Test").Infer(k, subs, trace), (All(List{Integer(2),Integer(3)})));
}

TEST(All, MatchingHypothesis)
{
	EXPECT_EQ((List{All("X")}).Matches(List{}, {}).Simplify(), (Equal{List{}, Variable("X")}));
	EXPECT_EQ((List{All("X")}).Matches(List{Integer(2)}, {}).Simplify(), (Equal{List{Integer(2)}, Variable("X")}));
	EXPECT_EQ((List{All("X")}).Matches(List{Integer(3),Integer(1)}, {}).Simplify(), (Equal{List{Integer(3),Integer(1)}, Variable("X")}));
	EXPECT_EQ((List{Integer(5), All("X")}).Matches(List{Integer(5),Integer(3)}, {}).Simplify(), (Equal{List{Integer(3),}, Variable("X")}));
}

TEST(All, MatchingSubstitution)
{
	Hypothesis substituted {Equal{Variable("Y"),List{Integer(2), Integer(3)}}};
	EXPECT_EQ((List{Integer(1), Integer(3)}).Matches(List{All("Y")}, substituted).Simplify(), False );
	EXPECT_EQ((List{Integer(2), Integer(3)}).Matches(List{All("Y")}, substituted).Simplify(), True );
	EXPECT_EQ((List{Integer(1), Integer(2), Integer(3)}).Matches(List{Integer(1), All("Y")}, substituted).Simplify(), True );
}

TEST(All, MatchAnonymous)
{
	EXPECT_EQ((List{All("")}).Matches(List{Integer(1)}, {}).Simplify(), True);
}

TEST(All, Hypothesis)
{
	Knowledge k{
		Predicate("cat", { Id("ginny")}) 
	};
	EXPECT_EQ(k.Infer(Predicate("cat", { All("C")})), (Association{
		True,
		Equal{Variable("C"), List{Id("ginny")}}
	}));
}

TEST(All, InferValue)
{
	Knowledge k;
	Trace trace;
	Hypothesis subs {Equal{Variable("X"),List{Integer(24)}},
						Equal{Set{Integer(25)},Variable("Y")}};
	EXPECT_EQ(All("X").Infer(k, subs, trace), (All(List{Integer(24)})));	
	EXPECT_EQ(All("Y").Infer(k, subs, trace), (All(Set{Integer(25)})));	
}

TEST(All, InferValliable)
{
	Knowledge k;
	Trace trace;
	Hypothesis subs {Equal{Variable("X"), (List{Integer(2)})}};
	EXPECT_EQ(All("Y").Infer(k, subs, trace), All("Y"));	
	EXPECT_EQ(All("").Infer(k, subs, trace), All(""));	
}

TEST(All, to_string)
{
    EXPECT_EQ(to_string(All(List{Integer(-3)})), "∀[-3]");
    EXPECT_EQ(to_string(All("ha")), "∀ha");
}



}

