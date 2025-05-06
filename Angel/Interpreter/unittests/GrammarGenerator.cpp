#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/Syntax.h"
#include <sstream>
#include <gtest/gtest.h>

namespace Interpreter
{

TEST(Grammar, GeneratorRule)
{
    std::stringstream source("bla");
    SymbolStream parse{{Symbol("syntax"),0,3}, {Symbol("rule"),0,3}, {Symbol("rule-name"),0,3}, 
        {Symbol("expression"), 3,0}, {Symbol("list"), 3,0}, 
        {Symbol("list-end"), 3,0}, {Symbol("expession-end"), 3,0}, {Symbol("syntax-end"), 3,0}};
    GrammarGenerator generator;
    const auto& syntax = generator(source, parse);
    EXPECT_EQ(syntax.Root(), Symbol("bla"));
}

TEST(Grammar, GeneratorTerms)
{
    std::stringstream source("nr::= \"23\"");
    SymbolStream parse{{Symbol("syntax"),0,10}, {Symbol("rule"),0,10}, {Symbol("rule-name"),0,2}, 
        {Symbol("expression"), 2, 8}, {Symbol("list"), 2, 8},
            {Symbol("Literal"), 6, 4},
        {Symbol("list-end"), 10, 0}, {Symbol("expession-end"), 10, 0}, {Symbol("syntax-end"), 10, 0}};
    GrammarGenerator generator;
    const auto& syntax = generator(source, parse);
    EXPECT_FALSE(syntax["nr"].empty());
    EXPECT_EQ(syntax["nr"].front().second.terms.front(), Term(Literal("23")));
}
}