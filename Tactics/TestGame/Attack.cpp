#include "pch.h"
#include "Game/Attack.h"
#include "Data.h"
#include "Fantasy.h"

namespace Game
{
namespace Test
{
TEST(Attack, Succeed)
{
	Fantasy world;
	Data::Knight attacker;
	Data::Victim victim;

	world.Imagine(attacker);
	world.Imagine(victim);

	const auto& part = victim.body.Anatomical()[L"All"];
	Attack attack(attacker, attacker.melee, victim, *attacker.melee.trajectory.begin(), &part);
	attack.Act(world);
	EXPECT_TRUE(victim.body.IsHurt());
	EXPECT_TRUE(victim.HasEvent(L"Hit"));
	EXPECT_TRUE(attacker.HasEvent(attacker.melee.name));
}

TEST(Attack, Mitigate)
{
	Fantasy world;
	Data::Knight attacker;
	Data::Victim victim;
	Data::PlotArmor armor;
	victim.Wear(armor);

	world.Imagine(attacker);
	world.Imagine(victim);

	const auto& part = victim.body.Anatomical()[L"All"];
	Attack attack(attacker, attacker.melee, victim, *attacker.melee.trajectory.begin(), &part);
	attack.Act(world);
	EXPECT_FALSE(victim.body.IsHurt());
	EXPECT_TRUE(victim.HasEvent(L"Mitigate"));
	EXPECT_TRUE(attacker.HasEvent(attacker.melee.name));
}

TEST(Attack, Miss)
{
	Fantasy world;
	Data::Knight attacker;
	Data::Victim victim;

	world.Imagine(attacker);
	world.Imagine(victim);

	Attack attack(attacker, attacker.melee, victim, *attacker.melee.trajectory.begin(), nullptr);
	attack.Act(world);
	EXPECT_FALSE(victim.body.IsHurt());
	EXPECT_TRUE(victim.HasEvent(L"Miss"));
	EXPECT_TRUE(attacker.HasEvent(attacker.melee.name));
}


}
}  // Game::Test
