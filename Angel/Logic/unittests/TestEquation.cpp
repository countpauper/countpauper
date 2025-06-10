#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Equation.h"
#include "Logic/Expression.h"
#include "Logic/Trace.h"

namespace Angel::Logic::Test
{

TEST(Equation, Construction)
{
    EXPECT_TRUE(Equation{}.empty());
    EXPECT_EQ(Equation{Integer(1)}.size(), 1);
    EXPECT_EQ(Expression(BinaryOperator(L'='), {Integer(1), Boolean(false)}), 
        (Equation{Integer(1), Boolean(false)}));   


	static_assert(Logic::IsOperation<Equation>);
}

TEST(Equation, Simplify)
{
    EXPECT_EQ((Equation{Id("cat")}).Simplify(), (Equation{Id("cat")}));
    EXPECT_EQ((Equation{Boolean(false), Boolean(true)}).Simplify(), Boolean(false));
    EXPECT_EQ((Equation{Integer(3), Integer(3)}).Simplify(), Boolean(true));
    EXPECT_EQ((Equation{Variable("y"), Integer(3)}).Simplify(), (Equation{Variable("y"), Integer(3)}));
    EXPECT_EQ((Equation{Integer(1), Variable("n"), Integer(2)}).Simplify(), Boolean(false));
    EXPECT_EQ((Equation{Id("cat"), Id("cat"), Variable("CAT")}).Simplify(),(Equation{Id("cat"), Variable("CAT")}));
}

TEST(Equation, Equations)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Equation{Integer(1), Integer(2)}), Boolean(false));
    EXPECT_EQ(k.Infer(Equation{Integer(1), Boolean(true)}), Boolean(true));
}

TEST(Equation, Nest)
{
    EXPECT_EQ((Equation{Integer(1), Equation{Integer(2)}}), 
        (Equation{Integer(1), Integer(2)}));
}

TEST(Equation, MatchEquationIsTheSameAsMatchExpression)
{
    EXPECT_EQ((Equation{Integer(3)}).Matches(Integer(3), {}).Simplify(), Boolean(true));
    EXPECT_EQ((Equation{Integer(1)}).Matches(Integer(2), {}).Simplify(), Boolean(false));
    EXPECT_EQ((Equation{Integer(-1)}).Matches(Variable("X"), {}).Simplify(), (Equation{Variable("X"), Integer(-1)}));
    EXPECT_EQ((Equation{Integer(-1)}).Matches(Variable("X"), {Equation{Variable("X"), Integer(2)}}).Simplify(), 
        Boolean(false));
}


TEST(Equation, Substitute)
{
	EXPECT_EQ((Equation{Integer(3), Variable("S")}).Substitute(Conjunction{Equation{Variable("S"), String("three")}}),
		(Equation{Integer(3), String("three")}));
}

TEST(Equation, Inference)
{
    Knowledge k { Predicate("cat") };
    EXPECT_EQ(k.Infer(Equation{Predicate("cat"), Boolean(true)}), Boolean(true));
}

TEST(Equation, AssumptionBecomesHypothesisWhenInferred)
{
    Hypothesis hypothesis;
    Knowledge k;
    Trace trace;
    EXPECT_EQ((Equation{Variable("X"), Integer(2)}).Infer(k, hypothesis, trace), 
        Association(Boolean(true), Equation{Variable("X"), Integer(2)}));
}

TEST(Equation, to_string)
{
    EXPECT_EQ(to_string(Equation{Integer(1)}), "=1");
    EXPECT_EQ(to_string(Equation{Variable("X"), Boolean(false)}), "$X=false");
}

}

