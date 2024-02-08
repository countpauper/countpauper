#include <gtest/gtest.h>
#include "Game/Character.h"

namespace Game::Test
{

TEST(Character, level_is_default_1)
{
    Race penguin("penguin");
    Character c("flo", penguin);

    EXPECT_EQ(c.Get(Stat::level).Total(), 1);
}

TEST(Character, race_gives_bonus)
{
    Race orc("orc", {{Stat::str, 2}});
    Character c("foe", orc);

    EXPECT_EQ(c.Get(Stat::str).Total(), 4);
}

TEST(Character, secondary_stat_is_derived_from_primary)
{
    Character::definition.clear();
    Race goblin("goblin");
    Character c("fop", goblin);
    Character::definition[Stat::str] = Stat("Strength", std::string_view());
    Character::definition[Stat::damage] = Stat("Damage", "", Stat::str, {0,0,1,1,2,2});

    EXPECT_EQ(c.Get(Stat::damage).Total(), 1);
    EXPECT_EQ(c.Get(Stat::damage).Description(), "1[Strength]");
}

TEST(Character, hitpoints_are_multiplied)
{
    Character::definition.clear();
    Race race("troll", {{Stat::con, 2}} );
    Character c("bar", race);
    c.Level(Stat::level, 3);
    Character::definition[Stat::con] = Stat("Con", "");
    Character::definition[Stat::hp] = Stat("Hitpoints", "", Stat::hd, Stat::multiply, Stat::level);
    Character::definition[Stat::hd] = Stat("Hit Dice", "", Stat::con, {4,4,5,5,6,6,7,7});
    EXPECT_EQ(c.Get(Stat::hp).Total(), 24);
    EXPECT_EQ(c.Get(Stat::hp).Description(), "24[Hit Dice]");
}

TEST(Character, capacity_is_carried)
{
    Character::definition.clear();
    Race race("troll", {{Stat::con, 2}} );
    Character c("bar", race);
    c.Level(Stat::level, 3);
    Character::definition[Stat::con] = Stat("Con", "");
    Character::definition[Stat::str] = Stat("Str", "");
    Character::definition[Stat::capacity] = Stat("Capacity", "", Stat::lift, Stat::add, Stat::endurance);
    Character::definition[Stat::endurance] = Stat("Endurance", "", Stat::con, {1,1,2,2,2,3,3});
    Character::definition[Stat::lift] = Stat("Lift", "", Stat::str, {1,1,1,2,2,3,3});
    EXPECT_EQ(c.Get(Stat::capacity).Total(), 3);
    EXPECT_EQ(c.Get(Stat::capacity).Description(), "1[Lift] + 2[Endurance]");
}

}
