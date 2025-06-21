#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Ordering.h"
#include "Logic/Expression.h"
#include "Logic/Trace.h"

namespace Angel::Logic::Test
{

TEST(Equal, Construction)
{
    EXPECT_TRUE(Equal{}.empty());
    EXPECT_EQ(Equal{Integer(1)}.size(), 1);
    EXPECT_EQ(Expression(Comparator(L'='), {Integer(1), Boolean(false)}), 
        (Equal{Integer(1), Boolean(false)}));   

	static_assert(Logic::is_operation<Equal>);
	static_assert(Logic::is_ordering<Equal>);
}

TEST(Equal, Simplify)
{
    EXPECT_EQ((Equal{Id("cat")}).Simplify(), (Equal{Id("cat")}));
    EXPECT_EQ((Equal{Boolean(false), Boolean(true)}).Simplify(), Boolean(false));
    EXPECT_EQ((Equal{Integer(3), Integer(3)}).Simplify(), Boolean(true));
    EXPECT_EQ((Equal{Variable("y"), Integer(3)}).Simplify(), (Equal{Variable("y"), Integer(3)}));
    EXPECT_EQ((Equal{Integer(1), Variable("n"), Integer(2)}).Simplify(), Boolean(false));
    EXPECT_EQ((Equal{Id("cat"), Id("cat"), Variable("CAT")}).Simplify(),(Equal{Id("cat"), Variable("CAT")}));
}

TEST(Equal, Equals)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Equal{Integer(1), Integer(2)}), Boolean(false));
    EXPECT_EQ(k.Infer(Equal{Integer(1), Boolean(true)}), Boolean(true));
}

TEST(Equal, Nest)
{
    EXPECT_EQ((Equal{Integer(1), Equal{Integer(2)}}), 
        (Equal{Integer(1), Integer(2)}));
}

TEST(Equal, MatchEqualIsTheSameAsMatchExpression)
{
    EXPECT_EQ((Equal{Integer(3)}).Matches(Integer(3), {}).Simplify(), Boolean(true));
    EXPECT_EQ((Equal{Integer(1)}).Matches(Integer(2), {}).Simplify(), Boolean(false));
    EXPECT_EQ((Equal{Integer(-1)}).Matches(Variable("X"), {}).Simplify(), (Equal{Variable("X"), Integer(-1)}));
    EXPECT_EQ((Equal{Integer(-1)}).Matches(Variable("X"), {Equal{Variable("X"), Integer(2)}}).Simplify(), 
        Boolean(false));
}


TEST(Equal, Substitute)
{
	EXPECT_EQ((Equal{Integer(3), Variable("S")}).Substitute(Conjunction{Equal{Variable("S"), String("three")}}),
		(Equal{Integer(3), String("three")}));
}

TEST(Equal, Inference)
{
    Knowledge k { Predicate("cat") };
    EXPECT_EQ(k.Infer(Equal{Predicate("cat"), Boolean(true)}), Boolean(true));
}

TEST(Equal, AssumptionBecomesHypothesisWhenInferred)
{
    Hypothesis hypothesis;
    Knowledge k;
    Trace trace;
    EXPECT_EQ((Equal{Variable("X"), Integer(2)}).Infer(k, hypothesis, trace), 
        Association(Boolean(true), Equal{Variable("X"), Integer(2)}));
}

TEST(Equal, to_string)
{
    EXPECT_EQ(to_string(Equal{Integer(1)}), "=1");
    EXPECT_EQ(to_string(Equal{Variable("X"), Boolean(false)}), "$X=false");
}

}

