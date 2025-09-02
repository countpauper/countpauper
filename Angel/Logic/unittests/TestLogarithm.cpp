#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Logarithm.h"
#include "Logic/Expression.h"
#include <cmath>

namespace Angel::Logic::Test
{

TEST(Logarithm, Construction)
{
    EXPECT_EQ(Logarithm({Integer(3)}), Logarithm({Integer(3)}));
    EXPECT_NE(Logarithm({Integer(3)}), Logarithm({Integer(3), Integer(2)}));
    EXPECT_EQ(GenericOperation(L'↓', Tuple{Integer(-2), Integer(-1)}), (Logarithm({Integer(-2), Integer(-1)})));   
}

TEST(Logarithm, Logarithms)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Logarithm({Real(2)})), Real(std::log(2)));
    EXPECT_EQ(k.Infer(Logarithm({Integer(16)})), Integer(4));
    EXPECT_EQ(k.Infer(Logarithm({Integer(8), Integer(2)})), Integer(3));
    // TODO complex EXPECT_EQ(k.Infer(Logarithm({Integer(2), Integer(-3)})), Integer(-1));
    EXPECT_EQ(k.Infer(Logarithm({True, Integer(2)})), Integer(0));
}

TEST(Logarithm, Simplify)
{
    EXPECT_EQ((Logarithm({Real(2.1)})).Simplify(), Real(std::log(2.1)));
    EXPECT_EQ((Logarithm({Integer(1000), Integer(10)})).Simplify(), Integer(3));
}

TEST(Logarithm, Substitute)
{
	EXPECT_EQ((Logarithm({Integer(2), Variable("I")}).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}})),
		(Logarithm({Integer(2), Integer(3)})));
}

TEST(Logarithm, Match)
{
	EXPECT_EQ(Logarithm({Integer(2), Integer(2)}).Matches(Integer(1), {}).Simplify(), True);
	EXPECT_EQ(Logarithm({Variable("Eight"), Integer(2)}).Matches(Integer(3), {Equal{Variable("Eight"), Integer(8)}}).Simplify(), True);
	// TODO inverse EXPECT_EQ((Logarithm({Integer(8), Variable("Two")}).Matches(Integer(3), {}).Simplify(), (Equal{Variable("Two"), Integer(2)})));
}

TEST(Logarithm, MatchArgument)
{
	EXPECT_EQ(Predicate("log", {Logarithm({Integer(1), Integer(2)})}).Matches(
                Predicate("log", {Integer(0)}),{}).Simplify(), True);
	EXPECT_EQ(Predicate("log", {Integer(4)}).Matches(
                Predicate("log", {Logarithm({Integer(16), Integer(2)})}),{}).Simplify(), True);
}

TEST(Logarithm, Inference)
{
    Knowledge k { Association(Predicate("cat"), Integer(2)) };
    EXPECT_EQ(k.Infer(Logarithm({Integer(4), Predicate("cat")})), Integer(2));
}

TEST(Logarithm, Solve)
{
    // Y = 2 ↓ X ->  X = 2 ↑ Y    
    EXPECT_EQ(Logarithm({Integer(2), Variable("X")}).Solve(Variable("X"), Variable("Y")), 
        Exponentiation({Integer(2),  Variable("Y")}));

    // Y = X ↓ 5 -> X = 5 ↑ (1/Y)
    EXPECT_EQ(Logarithm({Variable("X"), Integer(5)}).Solve(Variable("X"), Variable("Y")), 
        Exponentiation({Integer("5"), Division({Integer(1), Variable("Y")})}));
}


TEST(Logarithm, to_string)
{
    EXPECT_EQ(to_string(Logarithm({Integer(-3)})), "e↓-3");
    EXPECT_EQ(to_string(Logarithm({Integer(-2), Integer(3)})), "-2↓3");
    EXPECT_EQ(to_string(Logarithm({Integer(2), Integer(-3)})), "2↓-3");
}

}

