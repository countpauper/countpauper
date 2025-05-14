#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Conjunction.h"
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
    EXPECT_EQ(Conjunction{}.Infer(k), Boolean(true));
    EXPECT_EQ((Conjunction{Boolean(true)}).Infer(k), Boolean(true));
    EXPECT_EQ((Conjunction{Boolean(true), Boolean(false)}).Infer(k), Boolean(false));
    EXPECT_EQ((Conjunction{Conjunction{Boolean(true)}}).Infer(k), Boolean(true));
}

TEST(Conjunction, Nest)
{
    Knowledge k;
    EXPECT_EQ((Conjunction{Conjunction{Boolean(false)}, Conjunction{Boolean(true)}}).Infer(k), Boolean(false));
    EXPECT_EQ((Conjunction{Predicate("cat")}).Infer(k), Boolean(false));
}

TEST(Conjunction, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"));
    EXPECT_EQ((Conjunction{Predicate("cat")}).Infer(k), Boolean(true));
}

TEST(Conjunction, to_string)
{
    EXPECT_EQ(to_string(Conjunction{Boolean(true)}), "true");
    EXPECT_EQ(to_string(Conjunction{Boolean(true), Boolean(false)}), "true&false");
}

}

