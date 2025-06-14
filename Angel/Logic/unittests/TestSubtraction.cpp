#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Subtraction.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Subtraction, Construction)
{
    EXPECT_EQ((Subtraction{Integer(3)}), Subtraction({Integer(3)}));    
    EXPECT_NE((Subtraction{Integer(3)}), Subtraction({Integer(3), Integer(2)}));
    EXPECT_EQ(Expression(BinaryOperator(L'-'), {Integer(-2), Integer(-1)}), (Subtraction{Integer(-2), Integer(-1)}));   

	static_assert(Logic::is_operation<Subtraction>);

}

TEST(Subtraction, Subtractions)
{
    Knowledge k;

    EXPECT_EQ(k.Infer(Subtraction{Integer(2)}), Integer(2));
    EXPECT_EQ(k.Infer(Subtraction{Integer(2), Integer(3)}), Integer(-1));
    EXPECT_EQ(k.Infer(Subtraction{Integer(2), Integer(-3)}), Integer(5));
    EXPECT_EQ(k.Infer(Subtraction{Boolean(true), Integer(2)}), Integer(-1));
    EXPECT_EQ(k.Infer(Subtraction{Subtraction{Integer(2)}}), Integer(2));
}

TEST(Subtraction, Simplify)
{
    EXPECT_EQ((Subtraction{Integer(2)}).Simplify(), Integer(2));
    EXPECT_EQ((Subtraction{Integer(3), Integer(3)}).Simplify(), Integer(0));
}


TEST(Subtraction, Substitute)
{
	EXPECT_EQ((Subtraction{Integer(-2), Variable("I")}).Substitute(Conjunction{Equation{Variable("I"), Integer(3)}}),
		(Subtraction{Integer(-2), Integer(3)}));
}

TEST(Subtraction, MatchArgument)
{
	EXPECT_EQ(Predicate("sub", {Subtraction{Integer(1), Integer(2)}}).Matches(
                Predicate("sub", {Integer(-1)}),{}).Simplify(), Boolean(true));
	EXPECT_EQ(Predicate("sub", {Integer(-4)}).Matches(
                Predicate("sub", {Subtraction{Integer(-3), Boolean(true)}}),{}).Simplify(), Boolean(true));
}

TEST(Subtraction, Inference)
{
    Knowledge k { Association(Predicate("cat"), Integer(3)) };
    EXPECT_EQ(k.Infer(Subtraction{Integer(4), Predicate("cat")}), Integer(1));
}

TEST(Subtraction, to_string)
{
    EXPECT_EQ(to_string(Subtraction{Integer(-3)}), "-3");
    EXPECT_EQ(to_string(Subtraction{Integer(-2), Integer(3)}), "-2-3");
    EXPECT_EQ(to_string(Subtraction{Integer(2), Integer(-3)}), "2+3");
}

}

