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
    EXPECT_EQ(std::string(Rule({"S", {Literal("a"), Symbol("S")}})), "S::=\"a\" <S>");
}
TEST(Syntax, Lookup)
{
    Syntax syntax {
        Rule("operator", {Literal("<")}),
        Rule("operator", {Literal(">")}),
    };
    EXPECT_EQ(syntax.Lookup("operator").size(), 2);
    EXPECT_TRUE(syntax.Lookup("floperator").empty());
}

}