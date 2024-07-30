#include <gtest/gtest.h>
#include "Game/Attack.h"
#include "UI/Avatar.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Action, attack)
{
    Race race("goblin");

    Avatar attacker("a", race);
    Avatar target("t", race);
    Attack action(attacker, target);
    EXPECT_EQ(action.Description(), "Attack t");
}

}
