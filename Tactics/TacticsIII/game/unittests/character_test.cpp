#include <gtest/gtest.h>
#include "Game/Character.h"

namespace Game::Test
{

TEST(Character, Level)
{
    Race penguin("penguin");
    Character c("flo", penguin);

    EXPECT_EQ(c.Stat(Stat::Level), 1);
}

TEST(Character, RaceBonus)
{
    Race orc("orc");
    Character c("foe", orc);

    EXPECT_EQ(c.Stat(Stat::Str), 4);
}

}
