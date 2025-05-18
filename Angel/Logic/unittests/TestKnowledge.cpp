#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"

namespace Angel::Logic::Test
{

TEST(Knowledge, KnowSomething)
{
    Knowledge k;
    auto defaultSize = k.Root().size();
    EXPECT_EQ(k.Know(Predicate("ginny")), 1);
    EXPECT_EQ(k.Root().size(), defaultSize + 1);
    EXPECT_TRUE(k.Knows(Predicate("ginny")));
}

TEST(Knowledge, AlreadyKnows)
{
    Knowledge k;
    auto defaultSize = k.Root().size();
    EXPECT_EQ(k.Know(Predicate("ginny")), 1);
    EXPECT_EQ(k.Know(Predicate("ginny")), 0);
    EXPECT_EQ(k.Root().size(), defaultSize+1);
}


TEST(Knowledge, Defaults)
{
    Knowledge k;
    EXPECT_EQ(k.Root().size(), 2); 
    EXPECT_TRUE(k.Knows(Predicate("true")));
    EXPECT_EQ(k.Infer(Predicate("false")), Boolean(false));
}

}
