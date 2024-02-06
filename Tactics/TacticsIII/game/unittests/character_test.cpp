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
    Character::definition[Stat::Str] = Stat("Strength", "");
    Character::definition[Stat::Damage] = Stat("Damage", "", Stat::Str, {0,0,1,1,2,2});

    EXPECT_EQ(c.Stat(Stat::Damage).Total(), 1);
    EXPECT_EQ(c.Stat(Stat::Damage).Description(), "1[Strength]");
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
    EXPECT_EQ(c.Stat(Stat::HP).Description(), "24[Hit Dice]");
}

TEST(Character, capacity_is_carried)
{
    Character::definition.clear();
    Race race("troll", {{Stat::Con, 2}} );
    Character c("bar", race);
    c.Level(Stat::Level, 3);
    Character::definition[Stat::Capacity] = Stat("Capacity", "", Stat::Lift, Stat::Add, Stat::Endurance);
    Character::definition[Stat::Endurance] = Stat("Endurance", "", Stat::Con, {1,1,2,2,2,3,3});
    Character::definition[Stat::Lift] = Stat("Lift", "", Stat::Str, {1,1,1,2,2,3,3});
    EXPECT_EQ(c.Stat(Stat::Capacity).Total(), 3);
    EXPECT_EQ(c.Stat(Stat::Capacity).Description(), "1[Lift] + 2[Endurance]");
}

}
