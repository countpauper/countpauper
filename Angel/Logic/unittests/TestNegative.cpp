#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Negative.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Negative, Construction)
{
    EXPECT_TRUE(bool(Negative(Integer(0))));
    EXPECT_EQ(Negative(Integer(3)), Negative(Integer(3)));
    EXPECT_NE(Negative(Integer(3)), Negative(Integer(-3)));
    EXPECT_EQ(Expression(UnaryOperator(L'-'), {Integer(-2)}), Negative(Integer(-2)));
}

TEST(Negative, Simplify)
{
    EXPECT_EQ(Negative(Expression(Negative(Summation{Integer(2)}))).Simplify(), Integer(2));
    EXPECT_EQ(Negative(Summation{Integer(3)}).Simplify(), Negative(Integer(3)));
}

TEST(Negative, Substitute)
{
	EXPECT_EQ(Negative(Variable("P")).Substitute(Conjunction{Equation{Variable("P"), Integer(3)}}),
		Negative(Integer(3)));
}

TEST(Negative, Negations)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Negative(Integer(0))), Integer(0));
    EXPECT_EQ(k.Infer(Negative(Integer(2))), Integer(-2));
    EXPECT_EQ(k.Infer(Negative(Integer(-3))), Integer(3));
    EXPECT_EQ(k.Infer(Negative(Boolean(true))), Integer(-1));
}

TEST(Negative, to_string)
{
    EXPECT_EQ(to_string(Negative(Integer(3))), "-3");
    EXPECT_EQ(to_string(Negative(Integer(-2))), "--2");
}

}

