#include "pch.h"
#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"

namespace Angel::Logic::Test
{

TEST(TestElement, KnowSomething)
{
    Knowledge k;
    k.Know(predicate(L"ginny"));
    EXPECT_EQ(k.Clauses(), 1);
    EXPECT_TRUE(k.Knows(predicate(L"ginnny")));
}

TEST(TestElement, AlreadyKnows)
{
    Knowledge k;
    k.Know(predicate(L"ginny"));
    k.Know(predicate(L"ginny"));
    EXPECT_EQ(k.Clauses(), 1);
}

}
