#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Source.h"
#include <sstream>
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(Grammar, GenerateRule)
{
    Source source("<bla>");
    SymbolStream parse{{Symbol("syntax"),0,5}, {Symbol("rule"),0, 5}, {Symbol("rule-name"),0,5}, 
        {Symbol("expression"), 5,0}, {Symbol("list"), 5,0}, 
        {Symbol("list-end"), 5,0}, {Symbol("expession-end"), 5,0}, {Symbol("syntax-end"), 5,0}};
    GrammarGenerator generator;
    const auto& syntax = generator(source, parse);
    EXPECT_EQ(syntax.Root(), Symbol("bla"));
}

TEST(Grammar, GenerateTerms)
{
    Source  source("<nr>::= \"23\"");
    SymbolStream parse{{Symbol("syntax"),0,12}, {Symbol("rule"),0,12}, {Symbol("rule-name"),0,4}, 
        {Symbol("expression"), 8, 4}, {Symbol("list"), 8, 4}, {Symbol("Literal"), 8, 4},
        {Symbol("list-end"), 12, 0}, {Symbol("expession-end"), 12, 0}, {Symbol("syntax-end"), 12, 0}};
    GrammarGenerator generator;
    const auto& syntax = generator(source, parse);
    EXPECT_FALSE(syntax["nr"].empty());
    EXPECT_EQ(syntax["nr"].front().second.terms.front(), Term(Literal("23")));
}
}