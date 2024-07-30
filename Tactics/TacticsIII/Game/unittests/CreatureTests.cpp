#include <gtest/gtest.h>
#include "Game/Creature.h"
#include "CreatureDefinition.h"

namespace Game::Test
{

TEST(Creature, level_is_default_1)
{
    Race penguin("penguin");
    CreatureDefinition def;
    def.Define(Stat::level);

    Creature c("flo", penguin);

    EXPECT_EQ(c.Get(Stat::level).Total(), 1);
    EXPECT_EQ(c.Name(),"flo");
}

TEST(Creature, race_gives_bonus)
{
    Race orc("orc", {{Stat::str, 2}});
    CreatureDefinition def;
    def.Define(Stat::str);
    Creature c("foe", orc);

    EXPECT_EQ(c.Get(Stat::str).Total(), 4);
}

TEST(Creature, secondary_stat_is_derived_from_primary)
{
    Race goblin("goblin");
    Creature c("fop", goblin);

    CreatureDefinition def;
    def.Define(Stat::damage, Stat::str, 1.0/2);

    EXPECT_EQ(c.Get(Stat::damage).Total(), 1);
    EXPECT_EQ(c.Get(Stat::damage).Description(), "1[strength]");
}

TEST(Creature, hitpoints_are_added)
{
    CreatureDefinition def;
    def.Define(Stat::health, Stat::con, 2.5).Define(Stat::hp, Stat::level, Stat::add, Stat::health).Count();

    Race race("troll", {{Stat::con, 2}} );  // 2 + 2 = 4 con
    Creature c("bar", race);
    c.Level(Stat::level, 2); // 1 + 2 = 3 level
    EXPECT_EQ(c.Get(Stat::hp).Total(), 3+10);
    EXPECT_EQ(c.CounterAvailable(Stat::hp), c.Get(Stat::hp).Total());
    EXPECT_EQ(c.Get(Stat::hp).Description(), "3[level] + 10[health]");
}

TEST(Creature, damage_reduces_hitpoints)
{
    CreatureDefinition def;
    def.Define(Stat::hp, Stat::level, 1.0).Count();

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
