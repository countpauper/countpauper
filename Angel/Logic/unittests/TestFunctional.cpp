#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"


namespace Angel::Logic::Test
{

TEST(Functional, Factorial)
{
    Knowledge factorial {
        Association(Predicate("factorial", {LesserEqual{Integer(1)}}), Integer(1)),
        Association(Predicate("factorial", {Variable("n")}), 
            Multiplication{Variable("n"), Predicate("factorial", {Subtraction{Variable("n"), Integer(1)}})})
    };
    EXPECT_EQ(factorial.Infer(Predicate("factorial", {Integer(1)})), Integer(1));
    EXPECT_EQ(factorial.Infer(Predicate("factorial", {Integer(3)})), Integer(6));
}

}
