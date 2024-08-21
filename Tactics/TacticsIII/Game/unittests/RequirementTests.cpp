#include <gtest/gtest.h>
#include "Game/Requirement.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Requirement, not_equal)
{
    EXPECT_TRUE(Requirement(Stat::none, 1, Requirement::not_equal, 0));
    EXPECT_FALSE(Requirement(Stat::none, 1, Requirement::not_equal, 1));
    EXPECT_EQ(Requirement(Stat::ap, 3, Requirement::not_equal, 2).Description(), "action (3) is not 2");
    EXPECT_EQ(Requirement(Stat::ap, 3, Requirement::not_equal, 3).Description(), "action (3) is 3");
    EXPECT_TRUE((!Requirement(Stat::none, 2, Requirement::not_equal, 3)) == Requirement(Stat::none, 2, Requirement::equal, 3));
}

TEST(Requirement, equal)
{
    EXPECT_TRUE(Requirement(Stat::none, 1, Requirement::equal, 1));
    EXPECT_FALSE(Requirement(Stat::none, 1, Requirement::equal, 0));
    EXPECT_EQ(Requirement(Stat::ap, 3, Requirement::equal, 3).Description(), "action (3) is 3");
    EXPECT_EQ(Requirement(Stat::ap, 3, Requirement::equal, 2).Description(), "action (3) is not 2");
    EXPECT_TRUE((!Requirement(Stat::none, 2, Requirement::equal, 3)) == Requirement(Stat::none, 2, Requirement::not_equal, 3));
}

TEST(Requirement, less)
{
    EXPECT_TRUE(Requirement(Stat::none, 1, Requirement::less, 2));
    EXPECT_FALSE(Requirement(Stat::none, 3, Requirement::less, 2));
    EXPECT_EQ(Requirement(Stat::ap, 1, Requirement::less, 3).Description(), "action (1) is less than 3");
    EXPECT_EQ(Requirement(Stat::ap, 3, Requirement::less, 2).Description(), "action (3) is not less than 2");
    EXPECT_TRUE((!Requirement(Stat::none, 2, Requirement::less, 3)) == Requirement(Stat::none, 2, Requirement::greater_equal, 3));
}

TEST(Requirement, less_equal)
{
    EXPECT_TRUE(Requirement(Stat::none, 1, Requirement::less_equal, 2));
    EXPECT_TRUE(Requirement(Stat::none, 2, Requirement::less_equal, 2));
    EXPECT_FALSE(Requirement(Stat::none, 3, Requirement::less_equal, 2));
    EXPECT_EQ(Requirement(Stat::ap, 1, Requirement::less_equal, 3).Description(), "action (1) is not more than 3");
    EXPECT_EQ(Requirement(Stat::ap, 3, Requirement::less_equal, 1).Description(), "action (3) is more than 1");
    EXPECT_TRUE((!Requirement(Stat::none, 2, Requirement::less_equal, 3)) == Requirement(Stat::none, 2, Requirement::greater, 3));
}

TEST(Requirement, greater)
{
    EXPECT_TRUE(Requirement(Stat::none, 2, Requirement::greater, 1));
    EXPECT_FALSE(Requirement(Stat::none, 2, Requirement::greater, 3));
    EXPECT_EQ(Requirement(Stat::ap, 3, Requirement::greater, 1).Description(), "action (3) is more than 1");
    EXPECT_EQ(Requirement(Stat::ap, 2, Requirement::greater, 3).Description(), "action (2) is not more than 3");
    EXPECT_TRUE((!Requirement(Stat::none, 2, Requirement::greater, 3)) == Requirement(Stat::none, 2, Requirement::less_equal, 3));
}

TEST(Requirement, greater_equal)
{
    EXPECT_TRUE(Requirement(Stat::none, 2, Requirement::greater_equal, 1));
    EXPECT_TRUE(Requirement(Stat::none, 2, Requirement::greater_equal, 2));
    EXPECT_FALSE(Requirement(Stat::none, 2, Requirement::greater_equal, 3));
    EXPECT_EQ(Requirement(Stat::ap, 3, Requirement::greater_equal, 1).Description(), "action (3) is not less than 1");
    EXPECT_EQ(Requirement(Stat::ap, 1, Requirement::greater_equal, 3).Description(), "action (1) is less than 3");
    EXPECT_TRUE((!Requirement(Stat::none, 2, Requirement::greater_equal, 3)) == Requirement(Stat::none, 2, Requirement::less, 3));
}

TEST(Requirement, requirements_are_a_conjunection)
{
    Requirements reqs{
        Requirement(Stat::ap, 1, Requirement::greater, 0),
        Requirement(Stat::hp, 4, Requirement::less, 3)
    };
    EXPECT_FALSE(reqs);
    EXPECT_EQ(reqs.Description(), "action (1) is more than 0, but hitpoints (4) is not less than 3");
    EXPECT_EQ(reqs.Failed(), Requirements({reqs.back()}));
}
}

