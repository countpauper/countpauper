
#include <gtest/gtest.h>
#include "Engine/Interpret.h"

namespace Angel::Engine::Test
{

TEST(Interpretation, Axiom)
{
    Logic::Knowledge k; 
    Interpreter::Source source("cat");
    EXPECT_TRUE(Interpret(source, k).empty());
    EXPECT_FALSE(k.empty());
    EXPECT_TRUE(k.Knows(Logic::predicate("cat")));
}
}
