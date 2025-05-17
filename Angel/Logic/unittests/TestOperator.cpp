#include "Logic/Operator.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{

TEST(Operator, Construction)
{
    EXPECT_FALSE(Operator());
    EXPECT_TRUE(Operator(L'+'));
    EXPECT_EQ(Operator(L'&'), Operator("&", 2));
    EXPECT_EQ(Operator(L'¬'), Operator("~", 1));
}

TEST(Operator, String)
{
    EXPECT_EQ(std::string(Operator(L'&')), "&");
    EXPECT_EQ(std::string(Operator("!=", 2)), "≠");
}

TEST(Operator, Precedence)
{
    // TODO test pemdas and + is equal to min * equal to /
}

}
