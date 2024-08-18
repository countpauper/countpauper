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
    auto bonus = conditions.Bonus(Stat::speed);
    EXPECT_LT(bonus.Total(), 5);
}

TEST(Condition, ko_zeros_action_points)
{
    Conditions conditions;
    conditions.Apply<KO>();
    Computation ap(Engine::Range<int>(0, 1000));
    ap += Computation(10);
    ap += conditions.Bonus(Stat::ap);
    EXPECT_EQ(ap.Total(), 0);
}


TEST(Condition, serialize)
{

    Conditions conditions;
    conditions.Apply<Downed>();
    auto json = conditions.Serialize();
    Conditions deserialized(json);
    EXPECT_TRUE(deserialized.Is<Downed>());
}


}
