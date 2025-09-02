#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Division.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Division, Construction)
{
    EXPECT_EQ(Division({Integer(3)}), Division({Integer(3)}));
    EXPECT_NE(Division({Integer(3)}), Division({Integer(3), Integer(2)}));
    EXPECT_EQ(GenericOperation(L'÷', Tuple{Integer(-2), Integer(-1)} ), Division({Integer(-2), Integer(-1)}));   
}

TEST(Division, Divisions)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Division({Integer(2)})), Integer(2));
    EXPECT_EQ(k.Infer(Division({Integer(6), Integer(2)})), Integer(3));
    EXPECT_EQ(k.Infer(Division({Integer(9), Integer(-3)})), Integer(-3));
    EXPECT_EQ(k.Infer(Division({Integer(2), True})), Integer(2));
    EXPECT_EQ(k.Infer(Division({Division({Integer(2)})})), Integer(2));
}

TEST(Division, Simplify)
{
    EXPECT_EQ(Division({Integer(2)}).Simplify(), Integer(2));
    EXPECT_EQ(Division({Integer(3), Integer(3)}).Simplify(), Integer(1));
}


TEST(Division, Substitute)
{
	EXPECT_EQ(Division({Integer(-2), Variable("I")}).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}}),
		Division({Integer(-2), Integer(3)}));
}

TEST(Division, Solve)
{
    // Y = X / 2 / 3  => X = Y * 2 * 3 
    EXPECT_EQ(Division({Variable("X"), Integer(2), Integer(3)}).Solve(Variable("X"), Variable("Y")), 
        Multiplication({Variable("Y"), Integer(2), Integer(3)}));
    // Y = 2 / X / 3  => X = 2 / Y / 3
    EXPECT_EQ(Division({Integer(2), Variable("X"), Integer(3)}).Solve(Variable("X"), Variable("Y")), 
        Division({Integer(2), Variable("Y"), Integer(3)}));
}

TEST(Division, to_string)
{
    EXPECT_EQ(to_string(Division({Integer(-3)})), "-3");
    EXPECT_EQ(to_string(Division({Integer(-2), Integer(3)})), "-2÷3");
    EXPECT_EQ(to_string(Division({Integer(2), Integer(-3)})), "2÷-3");
    EXPECT_EQ(to_string(Division({Real(1.2), Multiplication({Integer(-3), Real(0.4)})})), "1.2÷(-3⋅0.4)");

}

}

