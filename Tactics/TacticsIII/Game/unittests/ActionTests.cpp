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
    EXPECT_CALL(actor.stats, Get(Stat::speed)).WillRepeatedly(Return(Computation(2)));
    EXPECT_CALL(actor.stats, Get(Stat::jump)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(actor.stats, Get(Stat::ap)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(actor.counts, Available(Stat::ap)).WillRepeatedly(Return(1));

    NiceMock<MockWorld> world;
    Move action(actor, world, Engine::Position(1,1,0));
    EXPECT_EQ(action.Description(), "Move (1, 1, 0)");
    EXPECT_EQ(action.AP(), 1);

    std::stringstream log;
    EXPECT_CALL(actor, Move(_,Engine::Position(1,1,0)));
    EXPECT_CALL(actor.counts, Cost(Stat::ap, 1, true));
    action.Execute(log);
    EXPECT_EQ(log.str(), "a moves to (1, 1, 0)\n");

}

TEST(Action, attack)
{
    MockActor attacker("a");
    MockActor target("t");
    Attack action(attacker, target);
    EXPECT_EQ(action.Description(), "Attack t");
    EXPECT_EQ(action.AP(), 1);

    EXPECT_CALL(attacker.stats, Get(Stat::offense)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(attacker.stats, Get(Stat::ap)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(target.stats, Get(Stat::hp)).WillRepeatedly(Return(Computation(1)));
    EXPECT_CALL(target.stats, Get(Stat::block)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::defense)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::dodge)).WillRepeatedly(Return(Computation(0)));

    std::stringstream log;
    action.Execute(log);
    EXPECT_EQ(attacker.GetCounts().Available(Stat::Id::ap), 0);
    EXPECT_EQ(target.GetCounts().Available(Stat::Id::hp), 0);
    EXPECT_EQ(log.str(), "a deals 1 damage to t\n");
}

}
