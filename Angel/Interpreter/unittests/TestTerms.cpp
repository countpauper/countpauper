#include "Interpreter/Terms.h"
#include "Interpreter/Source.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(Terms, Term)
{
    static_assert(sizeof(Term)<=64); 

    Term term = Literal("cat");
    EXPECT_EQ(std::to_string(term), "\"cat\"");
}

TEST(Terms, Literal)
{
    EXPECT_EQ(Literal("cat").Match(Source("catharsis").span()), 3);
    EXPECT_FALSE(Literal("cat").Match(Source("scat").span()));
    EXPECT_EQ(Literal("cat"), Literal("cat"));
    EXPECT_NE(Literal("cat"), Literal("catharsis"));
    EXPECT_THROW(Literal(""), std::invalid_argument);
    EXPECT_EQ(std::string(Literal("cat")), "\"cat\"");
    EXPECT_NE(std::hash<Literal>()(Literal("cat")), std::hash<Literal>()(Literal("catharsis")));
    static_assert(is_token<Literal>);
    static_assert(is_term<Literal>);
    static_assert(!is_token<int>);
}

TEST(Terms, Regex)
{
    EXPECT_FALSE(Regex("[0-9]*").Match(Source("cat").span()));
    EXPECT_EQ(Regex("[0-9]+").Match(Source("123/4").span()), 3);
    EXPECT_EQ(Regex("\\s+").Match(Source("\r\ncat").span()), 2);
    EXPECT_EQ(Regex("[abc]"), Regex("[abc]"));
    EXPECT_THROW(Regex(""), std::invalid_argument);

    EXPECT_EQ(std::string(Regex("cat")), "'cat'");
    
    static_assert(is_token<Regex>);
    static_assert(is_term<Regex>);
}

TEST(Terms, Epsilon)
{
    EXPECT_EQ(Epsilon("cat").Match(Source("cat").span()), 0);
    EXPECT_EQ(Epsilon("cat"), Epsilon("cat"));
    EXPECT_EQ(Epsilon("cat").GetSymbol(), Symbol("cat"));
    EXPECT_EQ(Epsilon(Symbol::epsilon).GetSymbol(), Symbol::epsilon);
#ifndef NDEBUG
    EXPECT_EQ(std::string(Epsilon("cat")), "ε<cat>");
#endif
    EXPECT_EQ(std::string(Epsilon(Symbol::epsilon)), "ε");

    static_assert(is_token<Epsilon>);
    static_assert(is_term<Epsilon>);
}

TEST(Terms, Hash)
{
    std::hash<Term> hasher;
    EXPECT_EQ(hasher(Literal("cat")), hasher(Literal("cat")));
    EXPECT_NE(hasher(Literal("cat")), hasher(Literal("Cat")));
    EXPECT_NE(hasher(Regex("()")), hasher(Literal("()")));
    EXPECT_NE(hasher(Literal("cat")), hasher(Epsilon("cat")));


}
}