#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"

namespace Angel::Logic::Test
{

TEST(TestElement, KnowSomething)
{
    Knowledge k;
    EXPECT_EQ(k.Know(predicate("ginny")), 1);
    EXPECT_EQ(k.Clauses(), 1);
    EXPECT_TRUE(k.Knows(predicate("ginny")));
}

TEST(TestElement, AlreadyKnows)
{
    Knowledge k;
    EXPECT_EQ(k.Know(predicate("ginny")), 1);
    EXPECT_EQ(k.Know(predicate("ginny")), 0);
    EXPECT_EQ(k.Clauses(), 1);
}

}
