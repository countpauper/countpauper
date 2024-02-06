#include <gtest/gtest.h>
#include "Game/Character.h"

namespace Game::Test
{

TEST(Character, level_is_default_1)
{
    Race penguin("penguin");
    Character c("flo", penguin);

    EXPECT_EQ(c.Stat(Stat::Level).Total(), 1);
}

TEST(Character, race_gives_bonus)
{
    Race orc("orc", {{Stat::Str, 2}});
    Character c("foe", orc);

    EXPECT_EQ(c.Stat(Stat::Str).Total(), 4);
}

TEST(Character, secondary_stat_is_derived_from_primary)
{
    Character::definition.clear();
    Race goblin("goblin");
    Character c("fop", goblin);
    Character::definition[Stat::Damage] = Stat("damage", "", Stat::Str, {0,0,1,1,2,2});

    EXPECT_EQ(c.Stat(Stat::Damage).Total(), 1);
}

TEST(Character, hitpoints_are_multiplied)
{
    Character::definition.clear();
    Race race("troll", {{Stat::Con, 2}} );
    Character c("bar", race);
    c.Level(Stat::Level, 3);
    Character::definition[Stat::HP] = Stat("Hitpoints", "", Stat::HD, Stat::Multiply, Stat::Level);
    Character::definition[Stat::HD] = Stat("Hit Dice", "", Stat::Con, {4,4,5,5,6,6,7,7});
    EXPECT_EQ(c.Stat(Stat::HP).Total(), 24);
}

}
