#include "Interpreter/Tokens.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(Tokens, Literal)
{
    EXPECT_TRUE(Literal("").IsEpsilon());
    EXPECT_FALSE(Literal("cat").IsEpsilon());
    EXPECT_EQ(Literal("cat").Match("catharsis"), 3);
    EXPECT_FALSE(Literal("cat").Match("scat"));
}



TEST(Tokens, Regex)
{
    EXPECT_FALSE(Literal(".+").IsEpsilon());
    EXPECT_FALSE(Literal(".*").IsEpsilon());
    EXPECT_FALSE(Regex("[0-9]*").Match("cat"));
    EXPECT_EQ(Regex("[0-9]+").Match("123/"), 3);
}


}