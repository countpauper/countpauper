#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Logarithm.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Logarithm, Construction)
{
    EXPECT_EQ((Logarithm{Integer(3)}), Logarithm({Integer(3)}));
    EXPECT_NE((Logarithm{Integer(3)}), Logarithm({Integer(3), Integer(2)}));
    EXPECT_EQ(Expression(MultiOperator(L'↓'), {Integer(-2), Integer(-1)}), (Logarithm{Integer(-2), Integer(-1)}));   

	static_assert(Logic::is_operation<Logarithm>);
}

TEST(Logarithm, Logarithms)
{
    Knowledge k;
    //EXPECT_EQ(k.Infer(Logarithm{Integer(2)}), Real(std::log(2)));
    EXPECT_EQ(k.Infer(Logarithm{Integer(8), Integer(2)}), Integer(3));
    // TODO complex EXPECT_EQ(k.Infer(Logarithm{Integer(2), Integer(-3)}), Integer(-1));
    EXPECT_EQ(k.Infer(Logarithm{True, Integer(2)}), Integer(0));
}

TEST(Logarithm, Simplify)
{
    EXPECT_EQ((Logarithm{Integer(2)}).Simplify(), Integer(2));
    EXPECT_EQ((Logarithm{Integer(3), Integer(3)}).Simplify(), Integer(1));
    //EXPECT_EQ((Logarithm{Integer(1), All(List{Integer(-2), Integer(3)})}).Simplify(), Integer(2));
}

TEST(Logarithm, Substitute)
{
	EXPECT_EQ((Logarithm{Integer(2), Variable("I")}).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}}),
		(Logarithm{Integer(2), Integer(3)}));
}

TEST(Logarithm, Match)
{
	EXPECT_EQ((Logarithm{Integer(2), Integer(2)}).Matches(Integer(1), {}).Simplify(), True);
	EXPECT_EQ((Logarithm{Variable("Eight"), Integer(2)}).Matches(Integer(3), {Equal{Variable("Eight"), Integer(8)}}).Simplify(), True);
//	EXPECT_EQ((Logarithm{Variable("One"), Integer(2)}).Matches(Integer(3), {}).Simplify(), (Equal{Variable("One"), Integer(1)}));
}

TEST(Logarithm, MatchArgument)
{
	EXPECT_EQ(Predicate("sum", {Logarithm{Integer(1), Integer(2)}}).Matches(
                Predicate("sum", {Integer(0)}),{}).Simplify(), True);
	EXPECT_EQ(Predicate("sum", {Integer(4)}).Matches(
                Predicate("sum", {Logarithm{Integer(16), Integer(2)}}),{}).Simplify(), True);
}

TEST(Logarithm, Inference)
{
    Knowledge k { Association(Predicate("cat"), Integer(2)) };
    EXPECT_EQ(k.Infer(Logarithm{Integer(4), Predicate("cat")}), Integer(2));
}

TEST(Logarithm, to_string)
{
    EXPECT_EQ(to_string(Logarithm{Integer(-3)}), "e↓-3");
    EXPECT_EQ(to_string(Logarithm{Integer(-2), Integer(3)}), "-2↓3");
    EXPECT_EQ(to_string(Logarithm{Integer(2), Integer(-3)}), "2↓-3");
}

}

