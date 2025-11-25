#include <gtest/gtest.h>
#include "Game/Requirement.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Requirement, stat_not_equal)
{
    EXPECT_TRUE(StatRequirement(Stat::none, 1, Comparator::not_equal, 0));
    EXPECT_FALSE(StatRequirement(Stat::none, 1, Comparator::not_equal, 1));
    EXPECT_EQ(StatRequirement(Stat::ap, 3, Comparator::not_equal, 2).Description(), "action (3) is not 2");
    EXPECT_EQ(StatRequirement(Stat::ap, 3, Comparator::not_equal, 3).Description(), "action (3) is 3");
    EXPECT_TRUE((!StatRequirement(Stat::none, 2, Comparator::not_equal, 3)) == StatRequirement(Stat::none, 2, Comparator::equal, 3));
}

TEST(Requirement, stat_equal)
{
    EXPECT_TRUE(StatRequirement(Stat::none, 1, Comparator::equal, 1));
    EXPECT_FALSE(StatRequirement(Stat::none, 1, Comparator::equal, 0));
    EXPECT_EQ(StatRequirement(Stat::ap, 3, Comparator::equal, 3).Description(), "action (3) is 3");
    EXPECT_EQ(StatRequirement(Stat::ap, 3, Comparator::equal, 2).Description(), "action (3) is not 2");
    EXPECT_TRUE((!StatRequirement(Stat::none, 2, Comparator::equal, 3)) == StatRequirement(Stat::none, 2, Comparator::not_equal, 3));
}

TEST(Requirement, stat_less)
{
    EXPECT_TRUE(StatRequirement(Stat::none, 1, Comparator::less, 2));
    EXPECT_FALSE(StatRequirement(Stat::none, 3, Comparator::less, 2));
    EXPECT_EQ(StatRequirement(Stat::ap, 1, Comparator::less, 3).Description(), "action (1) is less than 3");
    EXPECT_EQ(StatRequirement(Stat::ap, 3, Comparator::less, 2).Description(), "action (3) is not less than 2");
    EXPECT_TRUE((!StatRequirement(Stat::none, 2, Comparator::less, 3)) == StatRequirement(Stat::none, 2, Comparator::greater_equal, 3));
}

TEST(Requirement, stat_less_equal)
{
    EXPECT_TRUE(StatRequirement(Stat::none, 1, Comparator::less_equal, 2));
    EXPECT_TRUE(StatRequirement(Stat::none, 2, Comparator::less_equal, 2));
    EXPECT_FALSE(StatRequirement(Stat::none, 3, Comparator::less_equal, 2));
    EXPECT_EQ(StatRequirement(Stat::ap, 1, Comparator::less_equal, 3).Description(), "action (1) is not more than 3");
    EXPECT_EQ(StatRequirement(Stat::ap, 3, Comparator::less_equal, 1).Description(), "action (3) is more than 1");
    EXPECT_TRUE((!StatRequirement(Stat::none, 2, Comparator::less_equal, 3)) == StatRequirement(Stat::none, 2, Comparator::greater, 3));
}

TEST(Requirement, stat_greater)
{
    EXPECT_TRUE(StatRequirement(Stat::none, 2, Comparator::greater, 1));
    EXPECT_FALSE(StatRequirement(Stat::none, 2, Comparator::greater, 3));
    EXPECT_EQ(StatRequirement(Stat::ap, 3, Comparator::greater, 1).Description(), "action (3) is more than 1");
    EXPECT_EQ(StatRequirement(Stat::ap, 2, Comparator::greater, 3).Description(), "action (2) is not more than 3");
    EXPECT_TRUE((!StatRequirement(Stat::none, 2, Comparator::greater, 3)) == StatRequirement(Stat::none, 2, Comparator::less_equal, 3));
}

TEST(Requirement, stat_greater_equal)
{
    EXPECT_TRUE(StatRequirement(Stat::none, 2, Comparator::greater_equal, 1));
    EXPECT_TRUE(StatRequirement(Stat::none, 2, Comparator::greater_equal, 2));
    EXPECT_FALSE(StatRequirement(Stat::none, 2, Comparator::greater_equal, 3));
    EXPECT_EQ(StatRequirement(Stat::ap, 3, Comparator::greater_equal, 1).Description(), "action (3) is not less than 1");
    EXPECT_EQ(StatRequirement(Stat::ap, 1, Comparator::greater_equal, 3).Description(), "action (1) is less than 3");
    EXPECT_TRUE((!StatRequirement(Stat::none, 2, Comparator::greater_equal, 3)) == StatRequirement(Stat::none, 2, Comparator::less, 3));
}

TEST(Requirement, requirements_are_a_conjunection)
{
    Requirements reqs{
        StatRequirement(Stat::ap, 1, Comparator::greater, 0),
        PathRequirement(false),
        StatRequirement(Stat::hp, 4, Comparator::less, 3)
    };
    EXPECT_FALSE(reqs);
    EXPECT_EQ(reqs.Description(), "action (1) is more than 0, but the destination is not reachable and hitpoints (4) is not less than 3");
    EXPECT_EQ(reqs.Failed(), Requirements({reqs[1], reqs[2]}));
}
}

