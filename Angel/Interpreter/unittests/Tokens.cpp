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
    EXPECT_TRUE(epsilon.IsEpsilon());
    EXPECT_EQ(Literal(""), epsilon);
    EXPECT_EQ(Literal("cat"), Literal("cat"));
    EXPECT_NE(Literal("cat"), Literal("catharsis"));
    EXPECT_EQ(std::string(Literal("cat")), "\"cat\"");
    EXPECT_NE(Literal("cat").Hash(), Literal("catharsis").Hash());
}



TEST(Tokens, Regex)
{
    EXPECT_TRUE(Regex("").IsEpsilon());
    EXPECT_FALSE(Regex(".+").IsEpsilon());
    EXPECT_FALSE(Regex(".*").IsEpsilon());
    EXPECT_FALSE(Regex("[0-9]*").Match("cat"));
    EXPECT_EQ(Regex("[0-9]+").Match("123/4"), 3);
    EXPECT_EQ(Regex("\\s+").Match("\r\ncat"), 2);
    EXPECT_EQ(Regex("[abc]"), Regex("[abc]"));
    EXPECT_EQ(std::string(Regex("cat")), "'cat'");
    EXPECT_NE(Regex("()").Hash(), Literal("()").Hash());
}


}