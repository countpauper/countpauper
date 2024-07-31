#include <gtest/gtest.h>
#include "Game/Attack.h"
#include "Game/Mock/MockActor.h"
#include "CreatureDefinition.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Action, attack)
{
    MockActor attacker("a");
    MockActor target("t");
    Attack action(attacker, target);
    EXPECT_EQ(action.Description(), "Attack t");
    EXPECT_EQ(action.AP(), 1);

    EXPECT_CALL(attacker.stats, Get(Stat::offense)).WillRepeatedly(Return(StatDescriptor(1)));
    EXPECT_CALL(attacker.stats, Get(Stat::ap)).WillRepeatedly(Return(StatDescriptor(1)));
    EXPECT_CALL(target.stats, Get(Stat::hp)).WillRepeatedly(Return(StatDescriptor(1)));
    EXPECT_CALL(target.stats, Get(Stat::block)).WillRepeatedly(Return(StatDescriptor(0)));
    EXPECT_CALL(target.stats, Get(Stat::defense)).WillRepeatedly(Return(StatDescriptor(0)));
    EXPECT_CALL(target.stats, Get(Stat::dodge)).WillRepeatedly(Return(StatDescriptor(0)));

    std::stringstream log;
    action.Execute(log);
    EXPECT_EQ(attacker.GetCounts().Available(Stat::Id::ap), 0);
    EXPECT_EQ(target.GetCounts().Available(Stat::Id::hp), 0);
    EXPECT_EQ(log.str(), "a deals 1 damage to t\n");
}

}
