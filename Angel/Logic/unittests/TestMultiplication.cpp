#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Multiplication.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Multiplication, Construction)
{
    EXPECT_TRUE((Multiplication{}).empty());
    EXPECT_EQ((Multiplication{Integer(3)}), Multiplication({Integer(3)}));
    EXPECT_NE((Multiplication{Integer(3)}), Multiplication({Integer(3), Integer(2)}));
    EXPECT_EQ(Expression(MultiOperator(L'⋅'), {Integer(-2), Integer(-1)}), (Multiplication{Integer(-2), Integer(-1)}));   

	static_assert(Logic::is_operation<Multiplication>);
}

TEST(Multiplication, Multiplications)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Multiplication{Integer(2)}), Integer(2));
    EXPECT_EQ(k.Infer(Multiplication{Integer(2), Integer(3)}), Integer(6));
    EXPECT_EQ(k.Infer(Multiplication{Integer(3), Integer(-2)}), Integer(-6));
    EXPECT_EQ(k.Infer(Multiplication{True, Integer(2)}), Integer(2));
    EXPECT_EQ(k.Infer(Multiplication{Multiplication{Integer(2)}}), Integer(2));
}

TEST(Multiplication, Simplify)
{
    EXPECT_EQ((Multiplication{}).Simplify(), Integer(1));
    EXPECT_EQ((Multiplication{Integer(2)}).Simplify(), Integer(2));
    EXPECT_EQ((Multiplication{Integer(0), Integer(2)}).Simplify(), Integer(0));    
    EXPECT_EQ((Multiplication{Integer(3), Integer(3)}).Simplify(), Integer(9));
}


TEST(Multiplication, Substitute)
{
	EXPECT_EQ((Multiplication{Integer(-2), Variable("I")}).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}}),
		(Multiplication{Integer(-2), Integer(3)}));
}

TEST(Multiplication, to_string)
{
    EXPECT_EQ(to_string(Multiplication{Integer(-3)}), "-3");
    EXPECT_EQ(to_string(Multiplication{Integer(-2), Integer(3)}), "-2⋅3");
    EXPECT_EQ(to_string(Multiplication{Integer(2), Integer(-3)}), "2⋅-3");
    EXPECT_EQ(to_string(Multiplication{Integer(2), Summation({Integer(3), Integer(1)})}), "2⋅(3+1)");
}

}

