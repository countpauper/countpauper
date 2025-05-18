#include "Logic/Operator.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{

TEST(Operator, ConstructBinary)
{
    EXPECT_FALSE(BinaryOperator());
    EXPECT_TRUE(BinaryOperator(L'+'));
    EXPECT_EQ(BinaryOperator(L'&'), BinaryOperator("&"));
    EXPECT_NE(BinaryOperator(L'-'), UnaryOperator('-'));
}

TEST(Operator, ConstructUnary)
{
    EXPECT_FALSE(UnaryOperator());
    EXPECT_TRUE(UnaryOperator(L'+'));
    EXPECT_EQ(UnaryOperator(L'¬'), UnaryOperator("~"));
}


TEST(Operator, String)
{
    EXPECT_EQ(std::string(BinaryOperator(L'&')), "&");
    EXPECT_EQ(std::string(BinaryOperator("!=")), "≠");
}

TEST(Operator, Description)
{
    EXPECT_EQ(BinaryOperator(L'|').Description(), "or");
    EXPECT_EQ(UnaryOperator("@").Description(), "any of");
}

TEST(Operator, Precedence)
{
    // TODO test pemdas and + is equal to min * equal to /
}

}
