
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
    EXPECT_EQ(k.Knows(Logic::Predicate("cat", {Logic::Id("fish")})), Logic::True);
    EXPECT_EQ(k.Knows(Logic::Predicate("fish")), Logic::False);
}

TEST(Interpretation, Axioms)
{
    Logic::Knowledge k; 
    Interpreter::Source source("cat\nfish");
    AngelInterpreter interpreter;

    interpreter.Interpret(source, k);
    EXPECT_EQ(k.Knows(Logic::Predicate("cat")), Logic::True);
    EXPECT_EQ(k.Knows(Logic::Predicate("fish")), Logic::True);
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
    Interpreter::Source source("-1+2.0e-1?");
    EXPECT_EQ(interpreter.InterpretQuery(source), (
        Logic::Summation{Logic::Negative(Logic::Integer(1)), Logic::Real(0.2)}));
}

TEST(Interpretation, Precedence)
{
    AngelInterpreter interpreter;
    Interpreter::Source source("-1+2**3*4/5.0?");
    EXPECT_EQ(interpreter.InterpretQuery(source), (
        Logic::Summation{Logic::Negative(Logic::Integer(1)), 
            Logic::Division{
                Logic::Multiplication{
                    Logic::Exponentiation{Logic::Integer(2), Logic::Integer(3)}, 
                    Logic::Integer(4)
                }, Logic::Real(5.0)
            }
        }));
}

TEST(Interpretation, Braces)
{
    AngelInterpreter interpreter;
// both should work, but implementation only checks braces of rhs. Does that matter? Shouldn't pop back     Interpreter::Source source("(true-2)⋅3?");
    Interpreter::Source source("0⋅(true-2)⋅3?");
    EXPECT_EQ(interpreter.InterpretQuery(source), (
        Logic::Multiplication{Logic::Integer(0), Logic::Subtraction{Logic::Predicate("true"),  Logic::Integer(2)}, Logic::Integer(3)}));
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

TEST(Interpretation, Operation)
{
    Logic::Knowledge k; 
    AngelInterpreter interpreter;
    Interpreter::Source source("q:a&b");
    interpreter.Interpret(source, k);
    EXPECT_EQ(k.Infer(Logic::Predicate("q")), (Logic::Conjunction{Logic::Id("a"), Logic::Id("b")}));
}

TEST(Interpretation, BinaryOperation)
{
    Logic::Knowledge k; 
    AngelInterpreter interpreter;
    Interpreter::Source source("hascat(*C):$C.cat");
    interpreter.Interpret(source, k);
    EXPECT_EQ(k.Infer(Logic::Predicate("hascat", {Logic::Id("dog"), Logic::Id("cat")})), Logic::Integer(1));
}


TEST(Interpretation, Prefix)
{
    Logic::Knowledge k; 
    AngelInterpreter interpreter;
    Interpreter::Source source("neg($a):-$a");
    interpreter.Interpret(source, k);
    EXPECT_EQ(k.Infer(Logic::Predicate("neg",{Logic::Integer(3)})), Logic::Integer(-3));
}

TEST(Interpretation, PrefixComparator)
{
    Logic::Knowledge k; 
    AngelInterpreter interpreter;
    Interpreter::Source source("small(<=9)");
    interpreter.Interpret(source, k);
    EXPECT_EQ(k.Infer(Logic::Predicate("small",{Logic::Integer(8)})), Logic::True);
    EXPECT_EQ(k.Infer(Logic::Predicate("small",{Logic::Integer(10)})), Logic::False);
}

TEST(Interpretation, Factorial)
{
    Logic::Knowledge k; 
    AngelInterpreter interpreter;
    Interpreter::Source source("factorial(≤1)←1\nfactorial($n) ← $n⋅factorial($n-1)");
    interpreter.Interpret(source, k);
    EXPECT_EQ(k.Infer(Logic::Predicate("factorial",{Logic::Integer(0)})), Logic::Integer(1));
    EXPECT_EQ(k.Infer(Logic::Predicate("factorial",{Logic::Integer(5)})), Logic::Integer(120));
}

TEST(Interpretation, Sort)
{
    Logic::Knowledge k; 
    AngelInterpreter interpreter;
    Interpreter::Source source("sort($H,∀T)←sort(∀T:<$H) + [$H] + sort(∀T:≥$H)\nsort←[]");
    interpreter.Interpret(source, k);
    EXPECT_EQ(k.Infer(Logic::Predicate("sort",{
            Logic::Integer(5), Logic::Integer(2), Logic::Integer(1), Logic::Integer(4), Logic::Integer(3) })),
        (Logic::List{
            Logic::Integer(1), Logic::Integer(2), Logic::Integer(3), Logic::Integer(4), Logic::Integer(5) }));
}

}
