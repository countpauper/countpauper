#include <boost/test/unit_test.hpp>
#include "Game/Attack.h"
#include "Data.h"
#include "Fantasy.h"

namespace Game
{
namespace Test
{
BOOST_AUTO_TEST_SUITE(AttackTest);

BOOST_AUTO_TEST_CASE(Succeed)
{
	Fantasy world;
	Data::Knight attacker;
	Data::Victim victim;

	world.Imagine(attacker);
	world.Imagine(victim);

	const auto& part = victim.body.Anatomical()[L"All"];
	Attack attack(attacker, attacker.skill, victim, *attacker.skill.trajectory.begin(), &part);
	attack.Act(world);
	BOOST_CHECK(victim.body.IsHurt());
	BOOST_CHECK(victim.HasEvent(L"Hit"));
	BOOST_CHECK(attacker.HasEvent(attacker.skill.name));
}

BOOST_AUTO_TEST_CASE(Mitigate)
{
	Fantasy world;
	Data::Knight attacker;
	Data::Victim victim;
	Data::PlotArmor armor;
	victim.Wear(armor);

	world.Imagine(attacker);
	world.Imagine(victim);

	const auto& part = victim.body.Anatomical()[L"All"];
	Attack attack(attacker, attacker.skill, victim, *attacker.skill.trajectory.begin(), &part);
	attack.Act(world);
	BOOST_CHECK(!victim.body.IsHurt());
	BOOST_CHECK(victim.HasEvent(L"Mitigate"));
	BOOST_CHECK(attacker.HasEvent(attacker.skill.name));
}

BOOST_AUTO_TEST_CASE(Miss)
{
	Fantasy world;
	Data::Knight attacker;
	Data::Victim victim;

	world.Imagine(attacker);
	world.Imagine(victim);

	Attack attack(attacker, attacker.skill, victim, *attacker.skill.trajectory.begin(), nullptr);
	attack.Act(world);
	BOOST_CHECK(!victim.body.IsHurt());
	BOOST_CHECK(victim.HasEvent(L"Miss"));
	BOOST_CHECK(attacker.HasEvent(attacker.skill.name));
}



BOOST_AUTO_TEST_SUITE_END()
}
}  // Game::Test
