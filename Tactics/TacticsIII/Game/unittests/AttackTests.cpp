#include "Game/Attack.h"
#include "Game/Mock/MockActor.h"
#include "Game/Mock/MockWorld.h"
#include "Definition.h"
#include <gtest/gtest.h>

namespace Game::Test
{
using namespace ::testing;

TEST(Attack, hit)
{
    NiceMock<MockWorld> world;
    MockActor attacker("a");
    MockActor target("t");
    Attack action(world, attacker, target);
    EXPECT_EQ(action.Description(), "Attack t");
    EXPECT_EQ(action.AP(), 1);

    attacker.Move(world, {0,0,0});
    attacker.SetStats({
        {Stat::reach, 1},
        {Stat::sharp_damage, 2},
        {Stat::ap, 1}});
    target.Move(world, {1,0,0});

    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_EQ(log.str(), "a deals 2[sharp] damage to t\n");
    ASSERT_EQ(deltas.size(), 2);
    const auto& deltaAttacker = deltas[0];
    const auto& deltaTarget = deltas[1];
    EXPECT_EQ(deltaAttacker.Available(Stat::ap), 0);
    EXPECT_EQ(deltaTarget.Available(Stat::hp), 0);
}


TEST(Attack, out_of_reach)
{
    NiceMock<MockWorld> world;
    MockActor attacker("a");
    MockActor target("t");

    attacker.Move(world, {0,0,0});
    attacker.SetStats({
        {Stat::reach, 1},
        {Stat::ap, 1}});
    target.Move(world, {2,0,0});

    EXPECT_CALL(attacker.counts, Cost(_, _, _)).Times(0);

    Attack action(world, attacker, target);
    EXPECT_FALSE(action.CanDo());
    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "a can't attack t, because reach (1) is less than 2\n");
}

}
