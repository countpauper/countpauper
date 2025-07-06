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

TEST(Functional, Sort)
{
    Knowledge sort {
        Association(Predicate("sort", {}), List{}),
        Association(Predicate("sort", {Variable("H"), All("T")}), 
            Summation{
                Predicate("sort", {All{Association(All("T"), Lesser{Variable("H")})}}),
                List{Variable("H")},
                Predicate("sort", {All{Association(All("T"), GreaterEqual{Variable("H")})}})
            })
    };
    EXPECT_EQ(sort.Infer(Predicate("sort", {Integer(2), Integer(1)})), (List{Integer(1), Integer(2)}));
    EXPECT_EQ(sort.Infer(Predicate("sort", {Integer(2), Integer(1), Integer(5), Integer(3), Integer(4)})),
         (List{Integer(1), Integer(2), Integer(3), Integer(4), Integer(5)}));
}

}
