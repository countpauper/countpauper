#include <gtest/gtest.h>
#include "Game/Attack.h"
#include "Game/Move.h"
#include "Game/Mock/MockActor.h"
#include "Game/Mock/MockWorld.h"
#include "Definition.h"

namespace Game::Test
{
using namespace ::testing;


TEST(Action, move)
{
    MockActor actor("a");
    EXPECT_CALL(actor.stats, Get(Stat::speed, _, _)).WillRepeatedly(Return(Computation(2)));
    EXPECT_CALL(actor.stats, Get(Stat::jump, _, _)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(actor.stats, Get(Stat::ap, _, _)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(actor.counts, Available(Stat::ap)).WillRepeatedly(Return(1));

    NiceMock<MockWorld> world;
    EXPECT_CALL(world.map, GetBounds()).WillRepeatedly(Return(Engine::IntBox(Engine::Size(2,2,2))));
    Move action(world, actor, Engine::Position(1,1,0));
    EXPECT_EQ(action.Description(), "Move (1, 1, 0)");
    EXPECT_EQ(action.AP(), 1);
    EXPECT_TRUE(action.CanDo());

    std::stringstream log;
    auto delta = action.Execute(log).front();
    EXPECT_EQ(log.str(), "a moves to (1, 1, 0)\n");
    EXPECT_EQ(delta.Position(), Engine::Position(1,1,0));
    EXPECT_EQ(delta.Available(Stat::ap), 0);

}

TEST(Action, cant_move)
{
    MockActor actor("b");
    EXPECT_CALL(actor.stats, Get(Stat::speed, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(actor.stats, Get(Stat::jump, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(actor.stats, Get(Stat::ap, _, _)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(actor.counts, Available(Stat::ap)).WillRepeatedly(Return(1));
    NiceMock<MockWorld> world;

    Move action(world, actor, Engine::Position(1,1,0));
    EXPECT_FALSE(action.CanDo());
    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "b can't move, because speed (0) is 0\n");
}

TEST(Action, attack)
{
    NiceMock<MockWorld> world;
    MockActor attacker("a");
    MockActor target("t");
    Attack action(world, attacker, target);
    EXPECT_EQ(action.Description(), "Attack t");
    EXPECT_EQ(action.AP(), 1);

    EXPECT_CALL(target.stats, Get(Stat::block, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::dodge, _, _)).WillRepeatedly(Return(Computation(0)));

    EXPECT_CALL(attacker, Position()).WillRepeatedly(Return(Engine::Position(0,0,0)));
    EXPECT_CALL(attacker.stats, Get(Stat::reach, _, _)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(attacker.stats, Get(Stat::sharp_damage, _, _)).WillRepeatedly(Return(Computation(2)));
    EXPECT_CALL(attacker.stats, Get(Stat::blunt_damage, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(attacker.stats, Get(Stat::heat_damage, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(attacker.stats, Get(Stat::lightning_damage, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(attacker.stats, Get(Stat::cold_damage, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(attacker.stats, Get(Stat::poison_damage, _, _)).WillRepeatedly(Return(Computation(0)));

    EXPECT_CALL(target, Position()).WillRepeatedly(Return(Engine::Position(1,0,0)));
    EXPECT_CALL(target.stats, Get(Stat::blunt_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::sharp_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::heat_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::cold_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::lightning_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::poison_resist, _, _)).WillRepeatedly(Return(Computation(0)));

    EXPECT_CALL(attacker.counts, Available(Stat::ap)).WillRepeatedly(Return(1));

    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_EQ(log.str(), "a deals 2[sharp] damage to t\n");
    ASSERT_EQ(deltas.size(), 2);
    const auto& deltaAttacker = deltas[0];
    const auto& deltaTarget = deltas[1];
    EXPECT_EQ(deltaAttacker.Available(Stat::ap), 0);
    EXPECT_EQ(deltaTarget.Available(Stat::hp), 0);
}


TEST(Action, cant_attack)
{
    NiceMock<MockWorld> world;
    MockActor attacker("a");
    MockActor target("t");

    EXPECT_CALL(attacker, Position()).WillRepeatedly(Return(Engine::Position(0,0,0)));
    EXPECT_CALL(attacker.stats, Get(Stat::reach, _, _)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(target, Position()).WillRepeatedly(Return(Engine::Position(2,0,0)));

    EXPECT_CALL(attacker.counts, Available(Stat::ap)).WillRepeatedly(Return(1));
    EXPECT_CALL(attacker.counts, Cost(_, _, _)).Times(0);

    Attack action(world, attacker, target);
    EXPECT_FALSE(action.CanDo());
    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "a can't attack t, because reach (1) is less than 2\n");
}

}
