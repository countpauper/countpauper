#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Division.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(Division, Construction)
{
    EXPECT_EQ((Division{Integer(3)}), Division({Integer(3)}));
    EXPECT_NE((Division{Integer(3)}), Division({Integer(3), Integer(2)}));
    EXPECT_EQ(Expression(MultiOperator(L'÷'), {Integer(-2), Integer(-1)}), (Division{Integer(-2), Integer(-1)}));   

	static_assert(Logic::is_operation<Division>);
}

TEST(Division, Divisions)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Division{Integer(2)}), Integer(2));
    EXPECT_EQ(k.Infer(Division{Integer(6), Integer(2)}), Integer(3));
    EXPECT_EQ(k.Infer(Division{Integer(9), Integer(-3)}), Integer(-3));
    EXPECT_EQ(k.Infer(Division{Integer(2), True}), Integer(2));
    EXPECT_EQ(k.Infer(Division{Division{Integer(2)}}), Integer(2));
}

TEST(Division, Simplify)
{
    EXPECT_EQ((Division{Integer(2)}).Simplify(), Integer(2));
    EXPECT_EQ((Division{Integer(3), Integer(3)}).Simplify(), Integer(1));
}


TEST(Division, Substitute)
{
	EXPECT_EQ((Division{Integer(-2), Variable("I")}).Substitute(Conjunction{Equal{Variable("I"), Integer(3)}}),
		(Division{Integer(-2), Integer(3)}));
}

TEST(Division, to_string)
{
    EXPECT_EQ(to_string(Division{Integer(-3)}), "-3");
    EXPECT_EQ(to_string(Division{Integer(-2), Integer(3)}), "-2÷3");
    EXPECT_EQ(to_string(Division{Integer(2), Integer(-3)}), "2÷-3");
}

}

