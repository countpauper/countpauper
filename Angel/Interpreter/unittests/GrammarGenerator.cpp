#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/Syntax.h"
#include <sstream>
#include <gtest/gtest.h>

namespace Interpreter
{

TEST(Grammar, GeneratorRule)
{
    std::stringstream source("bla");
    SymbolStream parse{{Symbol("rule-name"),0,3}};
    GrammarGenerator generator;
    const auto& syntax = generator(source, parse);
    EXPECT_EQ(syntax.Root(), Symbol("bla"));
}


}