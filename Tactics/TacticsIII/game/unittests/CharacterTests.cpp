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

TEST(Character, hitpoints_are_added)
{
    Character::definition.clear();
    Race race("troll", {{Stat::con, 2}} );  // 2 + 2 = 4 con
    Character c("bar", race);
    c.Level(Stat::level, 2); // 1 + 2 = 3 level
    Character::definition[Stat::con] = Stat("Con", "");
    Character::definition[Stat::level] = Stat("Level", "");
    Character::definition[Stat::hp] = Stat("Hitpoints", "", Stat::level, Stat::add, Stat::health);
    Character::definition[Stat::health] = Stat("Health", "", Stat::con, {0,0,5,5,10,10,15,15});
    EXPECT_EQ(c.Get(Stat::hp).Total(), 3+10);
    EXPECT_EQ(c.Get(Stat::hp).Description(), "3[Level] + 10[Health]");
}

}
