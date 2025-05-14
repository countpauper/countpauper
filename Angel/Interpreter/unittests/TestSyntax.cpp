#include "Interpreter/Syntax.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(Syntax, EmptyLookup)
{
    Syntax emptySyntax;
    auto emptyLookup = emptySyntax.Lookup("cheese");
    EXPECT_TRUE(emptyLookup.empty());
}

TEST(Syntax, Rule)
{
#ifndef NDEBUG
    EXPECT_EQ(std::string(Rule({"S", {Literal("a"), Symbol("S")}})), "<S>::=\"a\" <S>");
#endif
}

TEST(Syntax, Lookup)
{
    Syntax syntax {
        Rule("operator", {Literal("<")}),
        Rule("operator", {Literal(">")}),
    };
    auto lookup  = syntax.Lookup(syntax.Root());
    EXPECT_EQ(std::distance(lookup.cbegin(), lookup.cend()), 2);
    EXPECT_TRUE(syntax.Lookup("pickle").empty());
}

}