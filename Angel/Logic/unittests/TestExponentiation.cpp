#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Exponentiation.h"
#include "Logic/Expression.h"
#include <cmath>

namespace Angel::Logic::Test
{

TEST(Exponentiation, Construction)
{
    EXPECT_EQ((Exponentiation{Integer(3)}), Exponentiation({Integer(3)}));
    EXPECT_NE((Exponentiation{Integer(3)}), Exponentiation({Integer(3), Integer(2)}));
    EXPECT_EQ(Expression(MultiOperator(L'↑'), {Integer(-2), Integer(-1)}), (Exponentiation{Integer(-2), Integer(-1)}));   

	static_assert(Logic::is_operation<Exponentiation>);
}

TEST(Exponentiation, Exponentiations)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Exponentiation{Integer(2)}), Real(std::exp(2)));
    EXPECT_EQ(k.Infer(Exponentiation{Integer(2), Integer(3)}), Integer(8));
    EXPECT_EQ(k.Infer(Exponentiation{Integer(2), Integer(-3)}), Real(1.0/8.0));
    EXPECT_DOUBLE_EQ(double(k.Infer(Exponentiation{Integer(-256), Integer(11)}).Get<Real>()), -3.09485009821345e26);
    EXPECT_EQ(k.Infer(Exponentiation{True, Integer(2)}), Integer(1));
    EXPECT_EQ(k.Infer(Exponentiation{Integer(2), Integer(3), Integer(2), Integer(1)}), Integer(64));
    EXPECT_EQ(k.Infer(Exponentiation{Integer(2), Exponentiation{Integer(2), Integer(2)}}), Integer(16));
}

TEST(Exponentiation, Simplify)
{
    EXPECT_EQ((Exponentiation{Integer(-3), Integer(3)}).Simplify(), Integer(-27));
    EXPECT_EQ((Exponentiation{Integer(2), All(List{Integer(2), Integer(3)})}).Simplify(), Integer(64));
}

TEST(Exponentiation, Substitute)
{
	EXPECT_EQ((Exponentiation{Integer(-2), Variable("I")}).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}}),
		(Exponentiation{Integer(-2), Integer(3)}));
}

TEST(Exponentiation, Match)
{
	EXPECT_EQ((Exponentiation{Integer(2), Integer(2)}).Matches(Integer(4), {}).Simplify(), True);
	EXPECT_EQ((Exponentiation{Variable("Three"), Integer(2)}).Matches(Integer(9), {Equal{Variable("Three"), Integer(3)}}).Simplify(), True);
}

TEST(Exponentiation, MatchArgument)
{
	EXPECT_EQ(Predicate("pow", {Exponentiation{Integer(2), Integer(2)}}).Matches(
                Predicate("pow", {Integer(4)}),{}).Simplify(), True);
	EXPECT_EQ(Predicate("pow", {Integer(-4)}).Matches(
                Predicate("pow", {Exponentiation{Integer(-4), True}}),{}).Simplify(), True);
}

TEST(Exponentiation, Inference)
{
    Knowledge k { Association(Predicate("cat"), Integer(2)) };
    EXPECT_EQ(k.Infer(Exponentiation{Integer(4), Predicate("cat")}), Integer(16));
}

TEST(Exponentiation, to_string)
{
    EXPECT_EQ(to_string(Exponentiation{Integer(-3)}), "e↑-3");
    EXPECT_EQ(to_string(Exponentiation{Integer(-2), Integer(3)}), "-2³");
    EXPECT_EQ(to_string(Exponentiation{Integer(2), Real(0.5)}), "√2");
    EXPECT_EQ(to_string(Exponentiation{Integer(2), Integer(-3)}), "2↑-3");
}

}

