
#include "Engine/Interpreter.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include <gtest/gtest.h>

namespace Angel::Engine::Test
{

TEST(Interpretation, Axiom)
{
    Logic::Knowledge k; 
    Interpreter::Source source("cat(fish)");
    AngelInterpreter interpreter;

    interpreter.Interpret(source, k);
    EXPECT_FALSE(k.Root().empty());
    EXPECT_TRUE(k.Knows(Logic::Predicate("cat", {Logic::Id("fish")})));
    EXPECT_FALSE(k.Knows(Logic::Predicate("fish")));
}

TEST(Interpretation, Axioms)
{
    Logic::Knowledge k; 
    Interpreter::Source source("cat , fish");
    AngelInterpreter interpreter;

    interpreter.Interpret(source, k);
    EXPECT_TRUE(k.Knows(Logic::Predicate("cat")));
    EXPECT_TRUE(k.Knows(Logic::Predicate("fish")));
}

TEST(Interpretation, Query)
{
    AngelInterpreter interpreter;
    Interpreter::Source source("cat ?");
    EXPECT_EQ(interpreter.InterpretQuery(source), Logic::Predicate("cat"));
}

TEST(Interpretation, Expression)
{
    AngelInterpreter interpreter;
    Interpreter::Source source("-1+2?");
    EXPECT_EQ(interpreter.InterpretQuery(source), (
        Logic::Summation{Logic::Negative(Logic::Integer(1)), Logic::Integer(2)}));
}

TEST(Interpretation, List)
{
    Logic::Knowledge k; 
    AngelInterpreter interpreter;
    Interpreter::Source source("l:[-1,2]");
    interpreter.Interpret(source, k);
    EXPECT_EQ(k.Infer(Logic::Predicate("l")), 
        (Logic::List{Logic::Negative(Logic::Integer(1)), Logic::Integer(2)}).Simplify());
}


TEST(Interpretation, Set)
{
    Logic::Knowledge k; 
    AngelInterpreter interpreter;
    Interpreter::Source source("s:{ginny, gizmo}");
    interpreter.Interpret(source, k);
    EXPECT_EQ(k.Infer(Logic::Predicate("s")), (Logic::Set{Logic::Id("ginny"), Logic::Id("gizmo")}));
}


}
