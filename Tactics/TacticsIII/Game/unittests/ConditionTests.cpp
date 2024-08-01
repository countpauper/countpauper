#include <gtest/gtest.h>
#include "Game/Conditions.h"

namespace Game::Test
{
using namespace ::testing;


TEST(Condition, apply)
{
    Conditions conditions;
    EXPECT_FALSE(conditions.Is<KO>());
    conditions.Apply<KO>();
    EXPECT_TRUE(conditions.Is<KO>());
    EXPECT_EQ(conditions.Description(), "KO");
}

TEST(Condition, apply_same)
{
    Conditions conditions;
    conditions.Apply<KO>();
    conditions.Apply<KO>();
    EXPECT_EQ(conditions.Description(), "KO");
}

TEST(Condition, down_reduces_speed)
{
    Conditions conditions;
    conditions.Apply<Downed>();
    StatDescriptor desc(5);
    auto& newDesc = conditions.Contribute(Stat::speed, desc);
    EXPECT_LT(newDesc.Total(), 5);
}

TEST(Condition, ko_zeros_action_points)
{
    Conditions conditions;
    conditions.Apply<KO>();
    StatDescriptor desc(Engine::Range<int>(0, 1000));
    desc.Contribute("Test", 10);
    auto& newDesc = conditions.Contribute(Stat::ap, desc);
    EXPECT_EQ(newDesc.Total(), 0);
}

}
