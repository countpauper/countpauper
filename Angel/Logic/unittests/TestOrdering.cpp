#include <gtest/gtest.h>
/*
#include "Logic/Knowledge.h"
#include "Logic/Ordering.h"
#include "Logic/Expression.h"
#include "Logic/Trace.h"

namespace Angel::Logic::Test
{

TEST(Ordering, Construction)
{
    EXPECT_TRUE(Lesser{}.empty());
    EXPECT_EQ(LesserEqual{Integer(1)}.size(), 1);
    EXPECT_EQ(Expression(BinaryOperator(L'>'), {Integer(1), Boolean(false)}), 
        (Greater{Integer(1), Boolean(false)}));   

	static_assert(Logic::IsOperation<GreaterEqual>);
}

TEST(Ordering, Order)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Equation{Integer(1), Integer(2)}), Boolean(false));
    EXPECT_EQ(k.Infer(Equation{Integer(1), Boolean(true)}), Boolean(true));
}

TEST(Ordering, Simplify)
{
    EXPECT_EQ((Lesser{Integer(0)}).Simplify(), Lesser{Integer(0)});
    EXPECT_EQ((Lesser{Boolean(true), Integer(2)}).Simplify(), Boolean(true));
    EXPECT_EQ((GreaterEqual{Integer(1), Integer(2)}).Simplify(), Boolean(false));
    EXPECT_EQ((Lesser{Integer(1), Integer(2), Variable("x")}).Simplify(), (Lesser{Integer(2), Variable("x")}));
    EXPECT_EQ((GreaterEqual{Variable("X"), Integer(4), Integer(4)}).Simplify(), (GreaterEqual{Variable("X"), Integer(4)}));
    EXPECT_EQ((Lesser{Variable("X"), Integer(4), Integer(3)}).Simplify(), Boolean(false));
    EXPECT_EQ((Greater{Variable("X"), Integer(9), Integer(1), Variable("Y")}).Simplify(), (Greater{Variable("X"), Integer(9), Integer(1),Variable("Y")}));
    EXPECT_EQ((LesserEqual{Integer(0), Integer(1), Variable("X"), Integer(2), Integer(3)}).Simplify(), (LesserEqual{Integer(1), Variable("X"), Integer(2)}));

}


TEST(Ordering, MatchUnaryOrderingIsTrueIfMatchIsOrdered)
{
    EXPECT_EQ((LesserEqual{Integer(3)}).Matches(Integer(3), {}).Simplify(), Boolean(true));
    EXPECT_EQ((Greater{Integer(3)}).Matches(Integer(4), {}).Simplify(), Boolean(false));
    EXPECT_EQ((LesserEqual{Integer(1)}).Matches(Integer(2), {}).Simplify(), Boolean(false));
    EXPECT_EQ((Lesser{Integer(-1)}).Matches(Variable("X"), {}).Simplify(), (Lesser{Variable("X"), Integer(-1)}));
}


TEST(Ordering, Substitute)
{
	EXPECT_EQ((Lesser{Integer(3), Variable("S")}).Substitute(Conjunction{Equation{Variable("S"), String("three")}}),
		(Lesser{Integer(3), String("three")}));
}

TEST(Ordering, Inference)
{
    Knowledge k { Association(Predicate("cat"), Integer(3)) };
    EXPECT_EQ(k.Infer(Greater{Predicate("cat"), Boolean(true)}), Boolean(true));
}

TEST(Ordering, AssumptionBecomesHypothesisWhenInferred)
{
    Hypothesis hypothesis;
    Knowledge k;
    Trace trace;
    EXPECT_EQ((Lesser{Variable("X"), Integer(2)}).Infer(k, hypothesis, trace), 
        Association(Boolean(true), Lesser{Variable("X"), Integer(2)}));
}

TEST(Ordering, to_string)
{
    EXPECT_EQ(to_string(LesserEqual{Integer(1)}), "≤1");
    EXPECT_EQ(to_string(Greater{Variable("X"), Integer(3)}), "$X>3");
    EXPECT_EQ(to_string(GreaterEqual{Integer(1), Integer(2), Integer(3)}), "1≥2≥3");
}

}
*/