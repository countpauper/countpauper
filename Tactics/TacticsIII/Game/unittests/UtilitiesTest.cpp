#include <gtest/gtest.h>
#include "Game/Utilities.h"
#include "Game/Mock/MockActor.h"
#include "Game/Mock/MockWorld.h"

namespace Game::Test
{

TEST(Utilities, AttackDistance)
{
    // Reach is the length of a melee weapon. 1 is the minimal and can hit touching blocks 
    // 2 can also hit diagonal 
    // 3 can hit across one block horizontally 
    // and so on, where at least up to 4,4 each manhattan distance increase is a reach increase 
    // This means 1 reach is about half a meter. 
    EXPECT_EQ(AttackDistance({0,0,0}), 0);
    EXPECT_GT(AttackDistance({1,0,0}), 0);
    EXPECT_EQ(AttackDistance({1,0,0}), AttackDistance({0,0,1}));
    EXPECT_GT(AttackDistance({1,1,0}), AttackDistance({0,1,0}));
    EXPECT_GT(AttackDistance({1,1,1}), AttackDistance({0,1,1}));
    EXPECT_GT(AttackDistance({2,0,0}), AttackDistance({1,1,0}));
    EXPECT_GT(AttackDistance({1,2,0}), AttackDistance({0,0,2}));
}

TEST(Utilities, VerticalReach)
{
    // Vertical reach is what is left (in meters) of the reach (in half meters) after reaching across to aim at elevated opponents over a delta vector
    // 0.25meter (a quarter square meter of attack surface) gives a solid base chance to hit and attack an adjacent opponent at a valid reach should be at least that much 
    // It longer reaches, the vertical reach also becomes more lenient although it stays around 50-66% of the distance
    EXPECT_GE(VerticalReach({0,0,0}, 0), 0.25f); 
    EXPECT_GE(VerticalReach({0,0,0}, 1), 0.75f); 
    EXPECT_GE(VerticalReach({1,0,0}, 1), 0.25f); 
    EXPECT_GE(VerticalReach({1,1,0}, 2), 0.5f); 
    EXPECT_GE(VerticalReach({0,0,2}, 3), 0.75f); 
    EXPECT_GE(VerticalReach({0,0,2}, 6), 2.25f); 
    EXPECT_GE(VerticalReach({0,2,0}, 4), 1.0f); 
    EXPECT_GE(VerticalReach({0,2,1}, 4), 1.0f); 
}

TEST(Utilities, Facing)
{
    Position to(2,2,2);
    EXPECT_EQ(Facing(Position{1,2,2}, to), Orientations{east});
    EXPECT_EQ(Facing(Position{3,2,2}, to), Orientations{west});
    EXPECT_EQ(Facing(Position{2,1,2}, to), Orientations{north});
    EXPECT_EQ(Facing(Position{2,3,2}, to), Orientations{south});
    EXPECT_EQ(Facing(Position{2,2,1}, to), Orientations{up});
    EXPECT_EQ(Facing(Position{2,2,3}, to), Orientations{down});
    EXPECT_EQ(Facing(Position{2,2,2}, to), Orientations{});

    EXPECT_EQ(Facing(Position{1,3,2}, to), (Orientations{east, south}));
    EXPECT_EQ(Facing(Position{2,3,1}, to), (Orientations{south, up}));

    EXPECT_EQ(Facing(Position{1,2,2.4}, to), (Orientations{east}));
    EXPECT_EQ(Facing(Position{1,2,2.6}, to), (Orientations{east, down}));
}

TEST(Utilities, AttackSurface)
{
    MockWorld world;
    MockActor attacker;
    MockActor defender;
    attacker.SetStats({{Stat::reach, 2}});
    world.map.SetHeightMap(Engine::Size(2,1,2), {0.0f, 0.0f});
    defender.Move(world, Position(1,0,0));
    EXPECT_CALL(attacker, GetSize()).WillOnce(Return(Size(1,1,1)));
    EXPECT_CALL(defender, GetSize()).WillOnce(Return(Size(1,1,2)));
    EXPECT_EQ(AttackHeight(world, attacker, defender), Engine::Range<ZType>(0, 2.0));
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
