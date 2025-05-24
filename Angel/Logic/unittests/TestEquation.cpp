#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Equation.h"
#include "Logic/Expression.h"


namespace Angel::Logic::Test
{

TEST(Equation, Construction)
{
    EXPECT_TRUE(Equation{}.empty());
    EXPECT_EQ(Equation{Integer(1)}.size(), 1);
    EXPECT_EQ(Expression(BinaryOperator(L'='), {Integer(1), Boolean(false)}), 
        (Equation{Integer(1), Boolean(false)}));   


	static_assert(Logic::IsOperation<Equation>);
}

TEST(Equation, Equations)
{
    Knowledge k;
    EXPECT_EQ(k.Infer(Equation{}), Boolean(true));
    EXPECT_EQ(k.Infer(Equation{Integer(1)}), Boolean(true));
    EXPECT_EQ(k.Infer(Equation{Integer(1), Integer(2)}), Boolean(false));
    EXPECT_EQ(k.Infer(Equation{Integer(1), Boolean(true)}), Boolean(true));
}

TEST(Equation, Nest)
{

    // EXPECT_EQ((Equation{Integer(1), Equation{Integer(2)}}), 
    //     (Equation{Integer(1), Integer(2)}));
}

TEST(Equation, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"));
    EXPECT_EQ(k.Infer(Equation{Predicate("cat")}), Boolean(true));
}

TEST(Equation, to_string)
{
    EXPECT_EQ(to_string(Equation{Integer(1)}), "1");
    EXPECT_EQ(to_string(Equation{Variable("X"), Boolean(false)}), "$X=false");
}

}

