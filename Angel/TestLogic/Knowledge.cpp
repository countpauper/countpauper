#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"

namespace Angel::Logic::Test
{

TEST(TestElement, KnowSomething)
{
    Knowledge k;
    k.Know(predicate("ginny"));
    EXPECT_EQ(k.Clauses(), 1);
    EXPECT_TRUE(k.Knows(predicate("ginnny")));
}

TEST(TestElement, AlreadyKnows)
{
    Knowledge k;
    k.Know(predicate("ginny"));
    k.Know(predicate("ginny"));
    EXPECT_EQ(k.Clauses(), 1);
}

}
