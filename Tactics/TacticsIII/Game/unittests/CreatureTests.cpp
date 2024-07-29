#include <gtest/gtest.h>
#include "Game/Creature.h"

namespace Game::Test
{

TEST(Creature, level_is_default_1)
{
    Race penguin("penguin");
    Creature::definition.clear();
    Creature::definition[Stat::level] = Stat("Level");
    Creature c("flo", penguin);

    EXPECT_EQ(c.Get(Stat::level).Total(), 1);
    EXPECT_EQ(c.Name(),"flo");
}

TEST(Creature, race_gives_bonus)
{
    Race orc("orc", {{Stat::str, 2}});
    Creature::definition[Stat::str] = Stat("Strength");
    Creature c("foe", orc);

    EXPECT_EQ(c.Get(Stat::str).Total(), 4);
}

TEST(Creature, secondary_stat_is_derived_from_primary)
{
    Creature::definition.clear();
    Race goblin("goblin");
    Creature c("fop", goblin);
    Creature::definition[Stat::str] = Stat("Strength", std::string_view());
    Creature::definition[Stat::damage] = Stat("Damage", "", Stat::str, {0,0,1,1,2,2});

    EXPECT_EQ(c.Get(Stat::damage).Total(), 1);
    EXPECT_EQ(c.Get(Stat::damage).Description(), "1[Strength]");
}

TEST(Creature, hitpoints_are_added)
{
    Creature::definition.clear();
    Creature::definition[Stat::con] = Stat("Con", "");
    Creature::definition[Stat::level] = Stat("Level", "");
    Creature::definition[Stat::hp] = Stat("Hitpoints", "", Stat::level, Stat::add, Stat::health);
    Creature::definition[Stat::health] = Stat("Health", "", Stat::con, {0,0,5,5,10,10,15,15});
    Creature::definition.Count(Stat::hp);

    Race race("troll", {{Stat::con, 2}} );  // 2 + 2 = 4 con
    Creature c("bar", race);
    c.Level(Stat::level, 2); // 1 + 2 = 3 level
    EXPECT_EQ(c.Get(Stat::hp).Total(), 3+10);
    EXPECT_EQ(c.CounterAvailable(Stat::hp), c.Get(Stat::hp).Total());
    EXPECT_EQ(c.Get(Stat::hp).Description(), "3[Level] + 10[Health]");
}

TEST(Creature, damage_reduces_hitpoints)
{
    Creature::definition.clear();
    Creature::definition[Stat::con] = Stat("Con", "");
    Creature::definition[Stat::level] = Stat("Level", "");
    Creature::definition[Stat::hp] = Stat("Hitpoints", "", Stat::level, Stat::add, Stat::health);
    Creature::definition.Count(Stat::hp);

    Race race("goblin");
    Creature c("fop", race);
    c.Level(Stat::level, 1);
    ASSERT_EQ(c.CounterAvailable(Stat::hp), 2);
    c.Damage(1);
    EXPECT_FALSE(c.Is<KO>());
    EXPECT_EQ(c.CounterAvailable(Stat::hp), 1);
    c.Damage(2);
    EXPECT_TRUE(c.Is<KO>());
    EXPECT_EQ(c.CounterAvailable(Stat::hp), 0);
}

}
