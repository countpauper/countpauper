#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Subtraction.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Subtraction, Construction)
{
    EXPECT_EQ((Subtraction({Integer(3)})), Subtraction({Integer(3)}));    
    EXPECT_NE((Subtraction({Integer(3)})), Subtraction({Integer(3), Integer(2)}));
    EXPECT_EQ(GenericOperation(L'-', {Integer(-2), Integer(-1)}), Subtraction({Integer(-2), Integer(-1)}));   
}

TEST(Subtraction, Subtractions)
{
    Knowledge k;

    EXPECT_EQ(k.Infer(Subtraction({Integer(2)})), Integer(2));
    EXPECT_EQ(k.Infer(Subtraction({Integer(2), Integer(3)})), Integer(-1));
    EXPECT_EQ(k.Infer(Subtraction({Integer(2), Integer(-3)})), Integer(5));
    EXPECT_EQ(k.Infer(Subtraction({True, Integer(2)})), Integer(-1));
    EXPECT_EQ(k.Infer(Subtraction({Subtraction({Integer(2)})})), Integer(2));
}

TEST(Subtraction, Simplify)
{
    EXPECT_EQ((Subtraction({Integer(2)})).Simplify(), Integer(2));
    EXPECT_EQ((Subtraction({Integer(3), Integer(3)})).Simplify(), Integer(0));
}


TEST(Subtraction, Substitute)
{
	EXPECT_EQ((Subtraction({Integer(-2), Variable("I")})).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}}),
		(Subtraction({Integer(-2), Integer(3)})));
}

TEST(Subtraction, MatchArgument)
{
	EXPECT_EQ(Predicate("sub", {Subtraction({Integer(1), Integer(2)})}).Matches(
                Predicate("sub", {Integer(-1)}),{}).Simplify(), True);
	EXPECT_EQ(Predicate("sub", {Integer(-4)}).Matches(
                Predicate("sub", {Subtraction({Integer(-3), True})}),{}).Simplify(), True);
}

TEST(Subtraction, Inference)
{
    Knowledge k { Association(Predicate("cat"), Integer(3)) };
    EXPECT_EQ(k.Infer(Subtraction({Integer(4), Predicate("cat")})), Integer(1));
}

TEST(Subtraction, Solve)
{
    // Y = X - 2 - 3  => X = Y + 2 + 3 
    EXPECT_EQ(Subtraction({Variable("X"), Integer(2), Integer(3)}).Solve(Variable("X"), Variable("Y")), 
        Summation({Variable("Y"), Integer(2), Integer(3)}));
    // Y = 2 - X - 3  => X = 2 - Y -3
    EXPECT_EQ(Subtraction({Integer(2), Variable("X"), Integer(3)}).Solve(Variable("X"), Variable("Y")), 
        Subtraction({Integer(2), Variable("Y"), Integer(3)}));
    // Y = X - [a] => X = Y + [a] (maybe it could also be just Y if there's no [a] in Y)
    EXPECT_EQ(Subtraction({Variable("X"), List{Id("a")}}).Solve(Variable("X"), Variable("Y")), 
        Summation({Variable("Y"), List{Id("a")}}));
}


TEST(Subtraction, to_string)
{
    EXPECT_EQ(to_string(Subtraction({Integer(-3)})), "-3");
    EXPECT_EQ(to_string(Subtraction({Integer(-2), Integer(3)})), "-2-3");
    EXPECT_EQ(to_string(Subtraction({Integer(2), Integer(-3)})), "2+3");
}

}

