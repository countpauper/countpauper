#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Summation.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Summation, Construction)
{
    EXPECT_TRUE(Summation{}.empty());
    EXPECT_EQ((Summation{Integer(3)}), Summation({Integer(3)}));
    EXPECT_NE((Summation{Integer(3)}), Summation({Integer(3), Integer(2)}));
    EXPECT_EQ(Expression(BinaryOperator(L'+'), {Integer(-2), Integer(-1)}), (Summation{Integer(-2), Integer(-1)}));   

	static_assert(Logic::is_operation<Summation>);
}

TEST(Summation, Summations)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Summation{}), Integer(0));
    EXPECT_EQ(k.Infer(Summation{Integer(2)}), Integer(2));
    EXPECT_EQ(k.Infer(Summation{Integer(2), Integer(3)}), Integer(5));
    EXPECT_EQ(k.Infer(Summation{Integer(2), Integer(-3)}), Integer(-1));
    EXPECT_EQ(k.Infer(Summation{True, Integer(2)}), Integer(3));
    EXPECT_EQ(k.Infer(Summation{Summation{Integer(2)}}), Integer(2));
}

TEST(Summation, Simplify)
{
    EXPECT_EQ((Summation{}).Simplify(), Integer(0));
    EXPECT_EQ((Summation{Integer(2)}).Simplify(), Integer(2));
    EXPECT_EQ((Summation{Integer(3), Integer(3)}).Simplify(), Integer(6));
}


TEST(Summation, Substitute)
{
	EXPECT_EQ((Summation{Integer(-2), Variable("I")}).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}}),
		(Summation{Integer(-2), Integer(3)}));
}

TEST(Summation, MatchArgument)
{
	EXPECT_EQ(Predicate("sum", {Summation{Integer(1), Integer(2)}}).Matches(
                Predicate("sum", {Integer(3)}),{}).Simplify(), True);
	EXPECT_EQ(Predicate("sum", {Integer(-4)}).Matches(
                Predicate("sum", {Summation{Integer(-5), True}}),{}).Simplify(), True);
}

TEST(Summation, Inference)
{
    Knowledge k { Association(Predicate("cat"), Integer(2)) };
    EXPECT_EQ(k.Infer(Summation{Integer(4), Predicate("cat")}), Integer(6));
}

TEST(Summation, to_string)
{
    EXPECT_EQ(to_string(Summation{Integer(-3)}), "-3");
    EXPECT_EQ(to_string(Summation{Integer(-2), Integer(3)}), "-2+3");
    EXPECT_EQ(to_string(Summation{Integer(2), Integer(-3)}), "2-3");
}

}

