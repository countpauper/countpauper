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

    CreatureDefinition def;
    def.Define(Stat::hp, 1).Define(Stat::ap, 1).Define(Stat::damage,1);

    ASSERT_EQ(target.GetStats().Get(Stat::Id::hp).Total(), 1);

    std::stringstream log;
    action.Execute(log);
    EXPECT_EQ(attacker.GetStats().Get(Stat::Id::ap).Total(), 0);
    EXPECT_EQ(target.GetStats().Get(Stat::Id::hp).Total(), 0);
}

}
