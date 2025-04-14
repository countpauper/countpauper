#include <gtest/gtest.h>
#include "Parser/LogicParser.h"
#include "Logic/Predicate.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"

namespace Angel::Parser::Test
{

TEST(TestElement, Id)
{
    std::stringstream s("cat");
    Logic::Object cat;
    s >> cat;
    EXPECT_EQ(cat, Logic::id("cat"));
}

TEST(TestElement, Ids)
{
    std::stringstream s("gizmo ginny 天使");
    Logic::Object gizmo, ginny, angel;
    s >> gizmo >> ginny >> angel;
    EXPECT_EQ(gizmo, Logic::id("gizmo"));
    EXPECT_EQ(ginny, Logic::id("ginny"));
    EXPECT_EQ(angel, Logic::id("天使"));
}

TEST(TestElement, Boolean)
{
    std::stringstream s("true false");
    Logic::Object yes, no;
    s >> yes >> no;

    EXPECT_EQ(yes, Logic::boolean(true));
    EXPECT_EQ(no, Logic::boolean(false));
}

TEST(TestElement, Integer)
{
    std::stringstream s("-9 23 cat_2");
    Logic::Object negative, positive, id;
    s >> negative >> positive >> id;

    EXPECT_EQ(positive, Logic::integer(23));
    EXPECT_EQ(negative, Logic::integer(-9));
    EXPECT_EQ(id, Logic::id("cat_2"));
}


}

