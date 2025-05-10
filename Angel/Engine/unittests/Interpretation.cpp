
#include <gtest/gtest.h>
#include "Engine/Interpreter.h"

namespace Angel::Engine::Test
{

TEST(Interpretation, Axiom)
{
    Logic::Knowledge k; 
    Interpreter::Source source("cat");
    AngelInterpreter interpreter;

    EXPECT_TRUE(interpreter.Interpret(source, k).empty());
    EXPECT_FALSE(k.empty());
    EXPECT_TRUE(k.Knows(Logic::predicate("cat")));
    EXPECT_FALSE(k.Knows(Logic::predicate("fish")));
}

TEST(Interpretation, Axioms)
{
    Logic::Knowledge k; 
    Interpreter::Source source("cat , fish");
    AngelInterpreter interpreter;

    EXPECT_TRUE(interpreter.Interpret(source, k).empty());
    EXPECT_TRUE(k.Knows(Logic::predicate("cat")));
    EXPECT_TRUE(k.Knows(Logic::predicate("fish")));
}

}
