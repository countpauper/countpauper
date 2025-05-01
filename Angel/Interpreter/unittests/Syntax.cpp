#include "Interpreter/Syntax.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(Syntax, EmptyLookup)
{
    Syntax emptySyntax;
    auto emptyLookup = emptySyntax.Lookup(123456);
    EXPECT_TRUE(emptyLookup.empty());
}

TEST(Syntax, Rule)
{
    EXPECT_EQ(std::string(Rule({"S", {Literal("a"), Symbol("S")}})), "S::=\"a\" <S>");
}
TEST(Syntax, Lookup)
{
    Syntax syntax {
        Rule("operator", {Literal("<")}),
        Rule("operator", {Literal(">")}),
    };
    auto lookup  = syntax.Lookup(syntax.front().Hash());
    EXPECT_EQ(std::distance(lookup.cbegin(), lookup.cend()), 2);
    EXPECT_TRUE(syntax.Lookup(123456).empty());
}

}