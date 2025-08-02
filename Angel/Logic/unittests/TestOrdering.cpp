#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include "Logic/Trace.h"

namespace Angel::Logic::Test
{

TEST(Ordering, Construction)
{
    EXPECT_TRUE(Lesser{}.empty());
    EXPECT_EQ(LesserEqual{Integer(1)}.size(), 1);
    EXPECT_EQ(Expression(Order(L'>'), {Integer(1), False}), 
        (Greater{Integer(1), False}));   
    EXPECT_EQ(Expression(Filter(L'≠'), {Integer(0)}), 
        (Unequal{Integer(0)}));   

	static_assert(Logic::is_operation<GreaterEqual>);
	static_assert(Logic::is_comparison<GreaterEqual>);
	static_assert(sizeof(Ordering(Lesser{}))<=32);
}

TEST(Ordering, Order)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Equal{Integer(1), Integer(2)}), False);
    EXPECT_EQ(k.Infer(Equal{Integer(1), True}), True);
}

TEST(Ordering, Simplify)
{
    EXPECT_EQ((Lesser{Integer(0)}).Simplify(), Lesser{Integer(0)});
    EXPECT_EQ((Lesser{True, Integer(2)}).Simplify(), True);
    EXPECT_EQ((GreaterEqual{Integer(1), Integer(2)}).Simplify(), False);
    EXPECT_EQ((Lesser{Integer(1), Integer(2), Variable("x")}).Simplify(), (Lesser{Integer(2), Variable("x")}));
    EXPECT_EQ((GreaterEqual{Variable("X"), Integer(4), Integer(4)}).Simplify(), (GreaterEqual{Variable("X"), Integer(4)}));
    EXPECT_EQ((Lesser{Variable("X"), Integer(4), Integer(3)}).Simplify(), False);
    EXPECT_EQ((Greater{Variable("X"), Integer(9), Integer(1), Variable("Y")}).Simplify(), (Greater{Variable("X"), Integer(9), Integer(1),Variable("Y")}));
    EXPECT_EQ((LesserEqual{Integer(0), Integer(1), Variable("X"), Integer(2), Integer(3)}).Simplify(), (LesserEqual{Integer(1), Variable("X"), Integer(2)}));

}


TEST(Ordering, MatchUnaryOrderingIsTrueIfMatchIsOrdered)
{
    EXPECT_EQ((LesserEqual{Integer(3)}).Matches(Integer(3), {}).Simplify(), True);
    EXPECT_EQ((Greater{Integer(4)}).Matches(Integer(3), {}).Simplify(), False);
    EXPECT_EQ((LesserEqual{Integer(1)}).Matches(Integer(2), {}).Simplify(), False);
    EXPECT_EQ((Lesser{Integer(-1)}).Matches(Variable("X"), {}).Simplify(), (Lesser{ Variable("X"), Integer(-1) }));
    EXPECT_EQ((Lesser{Integer(2)}).Matches(Variable("X"), {Greater{Variable("X"), Integer(0)}}).Simplify(), 
        (Conjunction{Greater{Variable("X"), Integer(0)}, Lesser{ Variable("X"), Integer(2) }}));
}


TEST(Ordering, Substitute)
{
	EXPECT_EQ((Lesser{Integer(3), Variable("S")}).Substitute(Conjunction{Equal{Variable("S"), String("three")}}),
		(Lesser{Integer(3), String("three")}));
}

TEST(Ordering, Inference)
{
    Knowledge k { Association(Predicate("cat"), Integer(3)) };
    EXPECT_EQ(k.Infer(Greater{Predicate("cat"), True}), True);
}

TEST(Ordering, AssumptionBecomesHypothesisWhenInferred)
{
    Hypothesis hypothesis;
    Knowledge k;
    Trace trace;
    EXPECT_EQ((Lesser{Variable("X"), Integer(2)}).Infer(k, hypothesis, trace), 
        Association(True, Lesser{Variable("X"), Integer(2)}));
}

TEST(Ordering, to_string)
{
    EXPECT_EQ(to_string(LesserEqual{Integer(1)}), "≤1");
    EXPECT_EQ(to_string(Greater{Variable("X"), Integer(3)}), "$X>3");
    EXPECT_EQ(to_string(GreaterEqual{Integer(1), Integer(2), Integer(3)}), "1≥2≥3");
}

}

