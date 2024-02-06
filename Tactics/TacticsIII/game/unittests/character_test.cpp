#include <gtest/gtest.h>
#include "Game/Character.h"

namespace Game::Test
{

TEST(Character, Level)
{
    Race penguin("penguin", {});
    Character c("flo", penguin);

    EXPECT_EQ(c.Stat(Stat::Level).Total(), 1);
}

TEST(Character, RaceBonus)
{
    Race orc("orc", {{Stat::Str, 2}});
    Character c("foe", orc);

    EXPECT_EQ(c.Stat(Stat::Str).Total(), 4);
}

}
