#include "Interpreter/Terms.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(Terms, Term)
{
    static_assert(sizeof(Term)<=128);   // TODO could be smaller without regex precompiling

    Term term = Literal("cat");
    EXPECT_EQ(std::to_string(term), "\"cat\"");
}

TEST(Terms, Literal)
{
    EXPECT_FALSE(Literal("cat").IsEpsilon());
    EXPECT_EQ(Literal("cat").Match("catharsis"), 3);
    EXPECT_FALSE(Literal("cat").Match("scat"));
    EXPECT_TRUE(epsilon.IsEpsilon());
    EXPECT_EQ(Literal(""), epsilon);
    EXPECT_EQ(Literal("cat"), Literal("cat"));
    EXPECT_NE(Literal("cat"), Literal("catharsis"));
    EXPECT_EQ(std::string(Literal("cat")), "\"cat\"");
    EXPECT_NE(std::hash<Literal>()(Literal("cat")), std::hash<Literal>()(Literal("catharsis")));
    static_assert(is_token<Literal>);
    static_assert(is_term<Literal>);
    static_assert(!is_token<int>);
}

TEST(Terms, Regex)
{
    EXPECT_FALSE(Regex(".+").IsEpsilon());
    EXPECT_FALSE(Regex(".*").IsEpsilon());
    EXPECT_FALSE(Regex("[0-9]*").Match("cat"));
    EXPECT_EQ(Regex("[0-9]+").Match("123/4"), 3);
    EXPECT_EQ(Regex("\\s+").Match("\r\ncat"), 2);
    EXPECT_EQ(Regex("[abc]"), Regex("[abc]"));
    EXPECT_EQ(std::string(Regex("cat")), "'cat'");
    EXPECT_NE(std::hash<Regex>()(Regex("()")), std::hash<Literal>()(Literal("()")));
    static_assert(is_token<Regex>);
    static_assert(is_term<Regex>);
}


TEST(Terms, EpsilonIsNothing)
{
    EXPECT_TRUE(IsEpsilon(epsilon));
    EXPECT_TRUE(IsEpsilon(Regex("")));
    EXPECT_TRUE(IsEpsilon(Literal("")));
    EXPECT_EQ(epsilon.Match("cheese"), 0);
}

}