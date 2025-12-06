#include <gtest/gtest.h>
#include "Game/Utilities.h"

namespace Game::Test
{

// Orientations Facing(Position from, Position& to);
// double ComputeCover(const World& world, const Actor& from, const Actor& to);
// double HitChance(const Actor& actor, const Actor& target);
// Computation ComputeDamage(const Computations& offense, const Computations& defense);

TEST(Utilities, ComputeDamage)
{
    EXPECT_EQ(ComputeDamage(Computations{{Stat::sharp_damage, 2}}, Computations{}).Total(), 2);
    EXPECT_EQ(ComputeDamage(Computations{{Stat::blunt_damage, 2}}, Computations{{Stat::blunt_resist, 1}}).Total(), 1);
    EXPECT_EQ(ComputeDamage(Computations{{Stat::blunt_damage, 2}}, Computations{{Stat::blunt_resist, 1}}).Description(), "1[blunt]");
    EXPECT_EQ(ComputeDamage(Computations{{Stat::blunt_damage, 2}, {Stat::lightning_damage, 1}}, Computations{{Stat::blunt_resist, 1}}).Description(), "1[blunt] + 1[lightning]");
    EXPECT_EQ(ComputeDamage(Computations{{Stat::heat_damage, 1}}, Computations{{Stat::heat_resist, 2}}).Total(), 0);
    EXPECT_EQ(ComputeDamage(Computations{{Stat::sharp_damage, 1}, {Stat::cold_damage,1}},
                            Computations{{Stat::cold_resist, 2}}).Total(), 1);
}
}
