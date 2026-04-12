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
    NiceMock<MockActor> attacker("a");
    NiceMock<MockActor> target("t");
    EXPECT_CALL(target, GetSize()).WillRepeatedly(Return(Size{0,0,1}));
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
    NiceMock<MockActor> attacker("a");
    NiceMock<MockActor> target("t");
    EXPECT_CALL(target, GetSize()).WillRepeatedly(Return(Size{0,0,1}));

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
    EXPECT_EQ(log.str(), "a can't attack t, because reach (1[reach]) is less than 3\n");
}

TEST(Attack, out_of_reach_due_to_height)
{
    NiceMock<MockWorld> world;
    NiceMock<MockActor> attacker("a");
    NiceMock<MockActor> lowTarget("low");
    EXPECT_CALL(lowTarget, GetSize()).WillRepeatedly(Return(Size{0,0,1}));    
    NiceMock<MockActor> highTarget("high");
    EXPECT_CALL(highTarget, GetSize()).WillRepeatedly(Return(Size{0,0,1}));    
    world.map.SetHeightMap({3,1,4}, {0.5, 1.1, 1.5});

    attacker.SetStats({
        {Stat::reach, 1},
        {Stat::ap, 1}});
    attacker.Move(world, {1,0,1.1});
    lowTarget.Move(world, {0,0,0.5});
    highTarget.Move(world, {2,0,1.5});
    ///              _   
    ///             / high      
    ///        __atk\_high 
    ///     low /atk  ==== 1.5 
    ///     low/========== 1.1
    ///     ============== 0.5
    /// ================== 0.0
    EXPECT_CALL(attacker.counts, Cost(_, _, _)).Times(0);

    Attack lowAttack(world, attacker, lowTarget);
    EXPECT_FALSE(lowAttack.CanDo());
    std::stringstream log;
    auto deltas = lowAttack.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "a can't attack low, because reach (1[reach]) is less than 2\n");

    Attack highAttack(world, attacker, highTarget);
    EXPECT_TRUE(highAttack.CanDo());
    deltas = highAttack.Execute(log);
    ASSERT_GE(deltas.size(), 0);
}

TEST(Attack, cover_at_reach)
{
    // A[]T
    NiceMock<MockWorld> world;
    NiceMock<MockActor> attacker("a");
    NiceMock<MockActor> target("t");
    EXPECT_CALL(target, GetSize()).WillRepeatedly(Return(Size{0,0,1}));
    world.map.SetHeightMap({3,1,4}, {0.1, 2.5, 0.2});
    attacker.Move(world, {0,0,0});
    attacker.SetStats({
        {Stat::reach, 3},
        {Stat::ap, 1}});
    target.Move(world, {2,0,0});

    Attack action(world, attacker, target);
    EXPECT_FALSE(action.CanDo());
    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "a can't attack t, because the target is behind cover\n");
}

TEST(Attack, partial_cover_due_to_diagonal_height)
{
    // A [ ]   
    //  \ T
    NiceMock<MockWorld> world;
    NiceMock<MockActor> attacker("a");
    NiceMock<MockActor> target("t");
    EXPECT_CALL(target, GetSize()).WillRepeatedly(Return(Size{0,0,1}));
    world.map.SetHeightMap({2,2,4}, {0.1, 4.0, 0.2, 0.3});
    attacker.Move(world, {0,0,0});
    attacker.SetStats({
        {Stat::reach, 3},
        {Stat::ap, 1}});
    target.Move(world, {1,1,0});

    Attack action(world, attacker, target);
    EXPECT_TRUE(action.CanDo());
    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_FALSE(deltas.empty());
}

TEST(Attack, cover_due_to_diagonal_height)
{
    // A []
    // [] T
    NiceMock<MockWorld> world;
    NiceMock<MockActor> attacker("a");
    NiceMock<MockActor> target("t");
    EXPECT_CALL(target, GetSize()).WillRepeatedly(Return(Size{0,0,1}));
    world.map.SetHeightMap({2,2,4}, {0.1, 4.0, 4.0, 0.1});
    attacker.Move(world, {0,0,0});
    attacker.SetStats({
        {Stat::reach, 3},
        {Stat::ap, 1}});
    target.Move(world, {1,1,0});

    Attack action(world, attacker, target);
    EXPECT_FALSE(action.CanDo());
    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "a can't attack t, because the target is behind cover\n");
}

TEST(Attack, cover_due_to_obstacle)
{
    // A []
    // [] T
    NiceMock<MockWorld> world;
    NiceMock<MockActor> attacker("a");
    NiceMock<MockActor> obstacle("o");
    NiceMock<MockActor> target("t");
    EXPECT_CALL(obstacle, GetSize()).WillRepeatedly(Return(Size{0,0,1.5}));
    EXPECT_CALL(target, GetSize()).WillRepeatedly(Return(Size{0,0,1}));
    world.map.SetHeightMap({3,1,4}, {0.3, 0.1, 0.2});
    attacker.Move(world, {0,0,0});
    attacker.SetStats({
        {Stat::reach, 3},
        {Stat::ap, 1}});
    obstacle.Move(world, {2,0,0});
    target.Move(world, {2,0,0});

    Attack action(world, attacker, target);
    EXPECT_FALSE(action.CanDo());
    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "a can't attack t, because the target is covered by o\n");
}


}
