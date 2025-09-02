#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Multiplication.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Multiplication, Construction)
{
    EXPECT_TRUE(Multiplication({}).empty());
    EXPECT_EQ(Multiplication({Integer(3)}), Multiplication({Integer(3)}));
    EXPECT_NE(Multiplication({Integer(3)}), Multiplication({Integer(3), Integer(2)}));
    EXPECT_EQ(GenericOperation(L'⋅', Tuple{Integer(-2), Integer(-1)}), Multiplication({Integer(-2), Integer(-1)}));   
}

TEST(Multiplication, Multiplications)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Multiplication({Integer(2)})), Integer(2));
    EXPECT_EQ(k.Infer(Multiplication({Integer(2), Integer(3)})), Integer(6));
    EXPECT_EQ(k.Infer(Multiplication({Integer(3), Integer(-2)})), Integer(-6));
    EXPECT_EQ(k.Infer(Multiplication({True, Integer(2)})), Integer(2));
    EXPECT_EQ(k.Infer(Multiplication({Multiplication({Integer(2)})})), Integer(2));
}

TEST(Multiplication, Simplify)
{
    EXPECT_EQ(Multiplication({}).Simplify(), Integer(1));
    EXPECT_EQ(Multiplication({Integer(2)}).Simplify(), Integer(2));
    EXPECT_EQ(Multiplication({Integer(0), Integer(2)}).Simplify(), Integer(0));    
    EXPECT_EQ(Multiplication({Integer(3), Integer(3)}).Simplify(), Integer(9));
}


TEST(Multiplication, Substitute)
{
	EXPECT_EQ((Multiplication({Integer(-2), Variable("I")}).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}})),
		(Multiplication({Integer(-2), Integer(3)})));
}


TEST(Multiplication, Solve)
{
    // Y = X * 2 * 3 => X = Y / 2 / 3  
    EXPECT_EQ(Multiplication({Variable("X"), Integer(2), Integer(3)}).Solve(Variable("X"), Variable("Y")), 
        Division({Variable("Y"), Integer(2), Integer(3)}));
    // Y = 2 * X * 3 => X = Y / 2 / 3 
    EXPECT_EQ(Multiplication({Integer(2), Variable("X"), Integer(3)}).Solve(Variable("X"), Variable("Y")), 
        Division({Variable("Y"), Integer(2), Integer(3)}));
}

TEST(Multiplication, to_string)
{
    EXPECT_EQ(to_string(Multiplication({Integer(-3)})), "-3");
    EXPECT_EQ(to_string(Multiplication({Integer(-2), Integer(3)})), "-2⋅3");
    EXPECT_EQ(to_string(Multiplication({Integer(2), Integer(-3)})), "2⋅-3");
    EXPECT_EQ(to_string(Multiplication({Integer(2), Summation({Integer(3), Integer(1)})})), "2⋅(3+1)");
}

}

