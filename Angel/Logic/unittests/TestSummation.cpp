#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Summation.h"
#include "Logic/Boolean.h"

namespace Angel::Logic::Test
{

TEST(Summation, Construction)
{
    EXPECT_TRUE(Summation{}.empty());
    EXPECT_EQ((Summation{Integer(3)}), Summation({Integer(3)}));
    EXPECT_NE((Summation{Integer(3)}), Summation({Integer(3), Integer(2)}));
    EXPECT_EQ(Expression(BinaryOperator(L'+'), {Integer(-2), Integer(-1)}), (Summation{Integer(-2), Integer(-1)}));   

	static_assert(Logic::IsOperation<Summation>);

}


TEST(Summation, Summations)
{
    Knowledge k;
    EXPECT_EQ(Summation{}.Infer(k), Integer(0));
    EXPECT_EQ((Summation{Integer(2)}).Infer(k), Integer(2));
    EXPECT_EQ((Summation{Integer(2), Integer(3)}).Infer(k), Integer(5));
    EXPECT_EQ((Summation{Integer(2), Integer(-3)}).Infer(k), Integer(-1));
    EXPECT_EQ((Summation{Boolean(true), Integer(2)}).Infer(k), Integer(3));
    EXPECT_EQ((Summation{Summation{Integer(2)}}).Infer(k), Integer(2));
}

TEST(Summation, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"), Integer(2));
    EXPECT_EQ((Summation{Integer(4), Predicate("cat")}).Infer(k), Integer(6));
}

TEST(Summation, to_string)
{
    EXPECT_EQ(to_string(Summation{Integer(-3)}), "-3");
    EXPECT_EQ(to_string(Summation{Integer(-2), Integer(3)}), "-2+3");
    EXPECT_EQ(to_string(Summation{Integer(2), Integer(-3)}), "2-3");
}

}

