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
    Move action(actor, world, Engine::Position(1,1,0));
    EXPECT_EQ(action.Description(), "Move (1, 1, 0)");
    EXPECT_EQ(action.AP(), 1);
    EXPECT_TRUE(action.CanDo());

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

    EXPECT_CALL(target.stats, Get(Stat::block, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::dodge, _, _)).WillRepeatedly(Return(Computation(0)));

    EXPECT_CALL(attacker.stats, Get(Stat::sharp_damage, _, _)).WillRepeatedly(Return(Computation(2)));
    EXPECT_CALL(attacker.stats, Get(Stat::blunt_damage, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(attacker.stats, Get(Stat::heat_damage, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(attacker.stats, Get(Stat::lightning_damage, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(attacker.stats, Get(Stat::cold_damage, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(attacker.stats, Get(Stat::poison_damage, _, _)).WillRepeatedly(Return(Computation(0)));

    EXPECT_CALL(target.stats, Get(Stat::blunt_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::sharp_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::heat_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::cold_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::lightning_resist, _, _)).WillRepeatedly(Return(Computation(0)));
    EXPECT_CALL(target.stats, Get(Stat::poison_resist, _, _)).WillRepeatedly(Return(Computation(0)));

    EXPECT_CALL(attacker.counts, Cost(Stat::ap, 1, false)).WillOnce(Return(1));
    //EXPECT_CALL(target.counts, Available(Stat::hp)).WillOnce(Return(1));
    EXPECT_CALL(target.counts, Cost(Stat::hp, 2, true)).WillOnce(Return(1));

    std::stringstream log;
    action.Execute(log);
    //EXPECT_EQ(attacker.GetCounts().Available(Stat::Id::ap), 0);
    //EXPECT_EQ(target.GetCounts().Available(Stat::Id::hp), 0);
    EXPECT_EQ(log.str(), "a deals 2[sharp] damage to t\n");
}

}
