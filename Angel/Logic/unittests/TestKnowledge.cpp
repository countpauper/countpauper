#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"

namespace Angel::Logic::Test
{

TEST(Knowledge, KnowSomething)
{
    Knowledge k;
    EXPECT_EQ(k.Know(Predicate("ginny")), 1);
    EXPECT_EQ(k.size(), 1);
    EXPECT_TRUE(k.Knows(Predicate("ginny")));
}

TEST(Knowledge, AlreadyKnows)
{
    Knowledge k;
    EXPECT_EQ(k.Know(Predicate("ginny")), 1);
    EXPECT_EQ(k.Know(Predicate("ginny")), 0);
    EXPECT_EQ(k.size(), 1);
}

}
