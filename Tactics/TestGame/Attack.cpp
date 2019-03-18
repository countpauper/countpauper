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
	Attack attack(attacker, victim, attacker.skill, part);
	attack.Act(world);
	BOOST_CHECK(victim.body.IsHurt());
}




BOOST_AUTO_TEST_SUITE_END()
}
}  // Game::Test
