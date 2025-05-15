#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Source.h"
#include "Interpreter/Grammar.h"
#include "Interpreter/BNF.h"
#include <sstream>
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(Grammar, GenerateRule)
{
    Source source("<bla>");
    SymbolStream parse{{Symbol("syntax"),0,5, &source}, {Symbol("rule"),0, 5, &source}, {Symbol("rule-name"),0,5, &source}, 
        {Symbol("expression"), 5,0, &source}, {Symbol("list"), 5,0, &source}, 
        {Symbol("list-end"), 5,0, &source}, {Symbol("expession-end"), 5,0, &source}, {Symbol("syntax-end"), 5,0, &source}};
    const auto& syntax = GenerateGrammar(source, parse);
    EXPECT_EQ(syntax.Root(), Symbol("bla"));
}

TEST(Grammar, GenerateTerms)
{
    Source  source("<nr>::= \"23\"");
    SymbolStream parse{{Symbol("syntax"),0,12}, {Symbol("rule"),0,12}, {Symbol("rule-name"),0,4, &source}, 
        {Symbol("expression"), 8, 4}, {Symbol("list"), 8, 4}, {Symbol("literal"), 8, 4, &source},
        {Symbol("list-end"), 12, 0}, {Symbol("expession-end"), 12, 0}, {Symbol("syntax-end"), 12, 0}};
    const auto& syntax = GenerateGrammar(source, parse);
    EXPECT_FALSE(syntax["nr"].empty());
    EXPECT_EQ(syntax["nr"].front().second.terms.front(), Term(Literal("23")));
}

TEST(Grammar, GenerateRegex)
{
    Source  source("<reg> ::='\\s+'");
    SymbolStream parse{{Symbol("syntax"),0,14}, {Symbol("rule"),0,14}, {Symbol("rule-name"),0,5, &source}, 
        {Symbol("expression"), 9, 5}, {Symbol("list"), 9, 5}, {Symbol("regex"), 9, 5, &source},
        {Symbol("list-end"), 14, 0}, {Symbol("expession-end"), 14, 0}, {Symbol("syntax-end"), 14, 0}};
    const auto& syntax = GenerateGrammar(source, parse);
    EXPECT_FALSE(syntax["reg"].empty());
    EXPECT_EQ(syntax["reg"].front().second.terms.front(), Term(Regex("\\s+")));
}


TEST(Grammar, GenerateEpsilon)
{
    Source  source("<ep>::= \"\"");
    SymbolStream parse{{Symbol("syntax"),0,12}, {Symbol("rule"),0,12}, {Symbol("rule-name"),0,4, &source}, 
        {Symbol("expression"), 8, 4}, {Symbol("list"), 8, 4}, {Symbol("literal"), 8, 2, &source},
        {Symbol("list-end"), 10, 0}, {Symbol("expession-end"), 10, 0}, {Symbol("syntax-end"), 10, 0}};
    const auto& syntax = GenerateGrammar(source, parse);
    EXPECT_FALSE(syntax["ep"].empty());
    EXPECT_EQ(syntax["ep"].front().second.terms.front(), Term(Epsilon(Symbol::epsilon)));
}

TEST(Grammar, FromSource)
{
    Source source("<a>::= \"a\" <b>\n<b>::=\"b\"");
    Grammar grammar(BNF);
    Syntax custom = grammar.Parse(source);
    EXPECT_EQ(custom.size(), 2);
    EXPECT_FALSE(custom["b"].empty());
    EXPECT_TRUE(custom["c"].empty());
}
}