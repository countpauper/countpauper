#include <gtest/gtest.h>
#include "Game/Utilities.h"
#include "Game/Mock/MockActor.h"
#include "Game/Mock/MockWorld.h"

namespace Game::Test
{

TEST(Utilities, Facing)
{
    Position to(2,2,2);
    EXPECT_EQ(Facing(Position{1,2,2}, to, Size(1,1,1)), Orientations{Orientation::east});
    EXPECT_EQ(Facing(Position{3,2,2}, to, Size(1,1,1)), Orientations{Orientation::west});
    EXPECT_EQ(Facing(Position{2,1,2}, to, Size(1,1,1)), Orientations{Orientation::north});
    EXPECT_EQ(Facing(Position{2,3,2}, to, Size(1,1,1)), Orientations{Orientation::south});
    EXPECT_EQ(Facing(Position{2,2,1}, to, Size(1,1,1)), Orientations{Orientation::up});
    EXPECT_EQ(Facing(Position{2,2,3}, to, Size(1,1,1)), Orientations{Orientation::down});
    EXPECT_EQ(Facing(Position{2,2,2}, to, Size(1,1,1)), Orientations{});

    EXPECT_EQ(Facing(Position{1,3,2}, to, Size(1,1,1)), (Orientations{Orientation::east, Orientation::south}));
    EXPECT_EQ(Facing(Position{2,3,1}, to, Size(1,1,1)), (Orientations{Orientation::north, Orientation::up}));

    EXPECT_EQ(Facing(Position{1,2,2.4}, to, Size(1,1,1)), (Orientations{Orientation::east}));
    EXPECT_EQ(Facing(Position{1,2,2.4}, to, Size(1,1,0.5)), (Orientations{Orientation::east, Orientation::down}));
    EXPECT_EQ(Facing(Position{1,2,2.6}, to, Size(1,1,1)), (Orientations{Orientation::east, Orientation::down}));
    EXPECT_EQ(Facing(Position{1,2,2.6}, to, Size(1,1,5)), (Orientations{Orientation::east}));

}

TEST(Utilities, Cover)
{
    MockWorld world;
    MockActor attacker;
    MockActor defender;
    EXPECT_LE(ComputeCover(world, attacker, defender), 1.0);
}

TEST(Utilities, HitChance)
{
    // TODO implement and test double HitChance(const Actor& actor, const Actor& target);
    MockActor attacker;
    MockActor defender;
    EXPECT_EQ(HitChance(attacker, defender), 1.0);
}

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
