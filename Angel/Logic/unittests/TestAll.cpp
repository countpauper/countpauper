#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/All.h"
#include "Logic/Expression.h"

namespace Angel::Logic::Test
{

TEST(All, Construction)
{
    EXPECT_TRUE(bool(All(List{})));
    // Is it equal ? becaues it's ordered and unordered. unique and non unique maybe if no duplicates 
    // EXPECT_EQ(All(List{Integer(3)}), All(Set{Integer(3)}));
    EXPECT_EQ(All(List{Integer(3)}), (All{Integer(3)}));
    EXPECT_NE(All(List{Integer(3)}), All(List{Integer(-3)}));
    EXPECT_EQ(Expression(PrefixOperator(L'∀'), Collection{List{Integer(-2)}}), All(List{Integer(-2)})); 
    EXPECT_EQ(Expression(PrefixOperator(L'∀'), Collection{Id("T")}), All("T"));
}

TEST(All, Simplify)
{
    EXPECT_EQ(All(List{Summation{Integer(2)}}).Simplify(), 
        All(List{Integer(2)}));
}

TEST(All, Substitute)
{
	EXPECT_EQ(All("P").Substitute(Conjunction{Equal{Variable("P"), List{Integer(3)}}}),
		All(List{Integer(3)}));
}

TEST(All, to_string)
{
    EXPECT_EQ(to_string(All(List{Integer(-3)})), "∀[-3]");
}

}

