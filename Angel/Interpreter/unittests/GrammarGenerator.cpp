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


}