#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Conjunction.h"
#include "Logic/Summation.h"
#include "Logic/Boolean.h"
#include "Logic/Predicate.h"

namespace Angel::Logic::Test
{

TEST(Conjunction, Construction)
{
    EXPECT_TRUE(Conjunction{}.empty());
    EXPECT_EQ(Conjunction{Boolean(false)}.size(), 1);    
}

TEST(Conjunction, Conjunctions)
{
    Knowledge k;
    EXPECT_EQ(Conjunction{}.Compute(k), Boolean(true));
    EXPECT_EQ((Conjunction{Boolean(true)}).Compute(k), Boolean(true));
    EXPECT_EQ((Conjunction{Boolean(true), Boolean(false)}).Compute(k), Boolean(false));
    EXPECT_EQ((Conjunction{Conjunction{Boolean(true)}}).Compute(k), Boolean(true));
}

TEST(Conjunction, Nest)
{
    Knowledge k;
    EXPECT_EQ((Conjunction{Conjunction{Boolean(false)}, Conjunction{Boolean(true)}}).Compute(k), Boolean(false));
    EXPECT_EQ((Conjunction{Predicate("cat")}).Compute(k), Boolean(false));
}

TEST(Conjunction, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"));
    EXPECT_EQ((Conjunction{Predicate("cat")}).Compute(k), Boolean(true));
}

TEST(Conjunction, to_string)
{
    EXPECT_EQ(to_string(Conjunction{Boolean(true)}), "true");
    EXPECT_EQ(to_string(Conjunction{Boolean(true), Boolean(false)}), "true&false");
}

TEST(Summations, Summations)
{
    Knowledge k;
    EXPECT_EQ(Summation{}.Compute(k), Integer(0));
    EXPECT_EQ((Summation{Integer(2)}).Compute(k), Integer(2));
    EXPECT_EQ((Summation{Integer(2), Integer(3)}).Compute(k), Integer(5));
    EXPECT_EQ((Summation{Integer(2), Integer(-3)}).Compute(k), Integer(-1));
    EXPECT_EQ((Summation{Boolean(true), Integer(2)}).Compute(k), Integer(3));
    EXPECT_EQ((Summation{Summation{Integer(2)}}).Compute(k), Integer(2));
}

TEST(Summation, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"), Integer(2));
    EXPECT_EQ((Summation{Integer(4), Predicate("cat")}).Compute(k), Integer(6));
}

TEST(Summation, to_string)
{
    EXPECT_EQ(to_string(Summation{Integer(-3)}), "-3");
    EXPECT_EQ(to_string(Summation{Integer(-2), Integer(3)}), "-2+3");
    EXPECT_EQ(to_string(Summation{Integer(2), Integer(-3)}), "2-3");
}

}

