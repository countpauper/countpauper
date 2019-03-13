#include <boost/test/unit_test.hpp>
#include <sstream>
#include "Game/Body.h"
#include "Game/Damage.h"
#include "Data.h"

namespace Game
{
namespace Test
{
BOOST_AUTO_TEST_SUITE(BodyTest);

BOOST_AUTO_TEST_CASE(SerializeSimple)
{
    Data::Simple d;
    Body b;
    d.stream >> b;
    BOOST_CHECK_EQUAL(b.Length(), 3);
    BOOST_CHECK_EQUAL(b.Strength(), 6);
    BOOST_CHECK_EQUAL(b.Dead(), false);
    BOOST_CHECK(b.Description() == L"Healthy");
}

BOOST_AUTO_TEST_CASE(SerializeHuman)
{
    Data::Human d;
    Body b;
    d.stream >> b;
    BOOST_CHECK_EQUAL(b.Length(), 5);
    BOOST_CHECK_EQUAL(b.Strength(), 10);
    BOOST_CHECK_EQUAL(b.Dead(), false);
    BOOST_CHECK(b.Description() == L"Healthy");
}

BOOST_AUTO_TEST_CASE(Copy)
{
    Data::Simple original;
    Body b = original;
    BOOST_CHECK_EQUAL(b.Length(), 3);
    BOOST_CHECK_EQUAL(b.Strength(), 6);
    BOOST_CHECK_EQUAL(b.Dead(), false);
    BOOST_CHECK(b.Description() == L"Healthy");
}

BOOST_AUTO_TEST_CASE(Hurt)
{
    Data::Simple b;
    b[L"All"].Hurt(Damage(Wound::Type::Sharp, Score(L"", 5)));
    BOOST_CHECK(b.Description() == L"All=Grazed");
    BOOST_CHECK_EQUAL(b.Intelligence(), 5);
    BOOST_CHECK_EQUAL(b.Strength(), 5);
}

BOOST_AUTO_TEST_CASE(Dead)
{
    Data::Simple b;
    b[L"All"].Hurt(Damage(Wound::Type::Blunt, Score(L"", 20)));
    BOOST_CHECK(b.Description() == L"All=Shattered");
    BOOST_CHECK_EQUAL(b.Intelligence(), 0);
    BOOST_CHECK(b.Dead());
}

BOOST_AUTO_TEST_CASE(Weapon)
{
    Data::Human b;
    auto& arm = b[L"RArm"];
    BOOST_CHECK(arm.Held()==nullptr);
    Data::Blade weapon;
    arm.Hold(weapon);
    BOOST_CHECK(arm.Held() == &weapon);
    arm.Drop();
    BOOST_CHECK(arm.Held()==nullptr);
    BOOST_CHECK_THROW(b[L"Legs"].Hold(weapon), std::exception);
    auto& otherArm = b[L"LArm"];
    otherArm.Hold(weapon);
    arm.Hold(weapon);
    BOOST_CHECK(arm.Held()==&weapon);
    // TODO: how, no ptr back from arm, weapon doesn't keep. pbly should for transfering from another body 
    // Enable this for disarm/stealing 
    // BOOST_CHECK(otherArm.Held()==nullptr);
}

BOOST_AUTO_TEST_CASE(Engage)
{
    Data::Human b;
    auto arm = b[L"RArm"];
    Data::Blade weapon;
    arm.Hold(weapon);
    Data::Melee first;
    Data::Combo combo;
    BOOST_CHECK(b.Ready(first));
    b.Engage(first);
    BOOST_CHECK(!b.Ready(first));
    BOOST_CHECK(b.Ready(combo));
    b.Disengage();
    BOOST_CHECK(b.Ready(first));
}


BOOST_AUTO_TEST_CASE(Grip)
{
    Data::Human b;
    BOOST_CHECK(b[L"RArm"].Grip());
    BOOST_CHECK(b[L"LArm"].Grip());
    BOOST_CHECK(!b[L"Legs"].Grip());
}

BOOST_AUTO_TEST_CASE(Vital)
{
    Data::Human b;
    BOOST_CHECK(b[L"Head"].IsVital());
    BOOST_CHECK(b[L"Chest"].IsVital());
    BOOST_CHECK(!b[L"Belly"].IsVital());
    BOOST_CHECK(!b[L"RArm"].IsVital());
    BOOST_CHECK(!b[L"LArm"].IsVital());
    BOOST_CHECK(!b[L"Legs"].IsVital());
}

BOOST_AUTO_TEST_CASE(UseDoubleHand)
{
	Data::Human b;
	auto& arm = b[L"RArm"];
	Data::Blade weapon;
	arm.Hold(weapon);
	Data::Melee melee;
	auto& limbs = b.UsedLimbs(melee);
	BOOST_CHECK(limbs.count(&arm) == 1);
	BOOST_CHECK(limbs.count(&b[L"LArm"]) == 1);
	auto used = b.UsedWeapon(melee);
	BOOST_CHECK_EQUAL(used.first, &arm);
	BOOST_CHECK_EQUAL(used.second, &weapon);
}

BOOST_AUTO_TEST_CASE(UseSingleHand)
{
	Data::Human b;
	auto& armR = b[L"RArm"];
	auto& armL = b[L"LArm"];
	Data::Blade weapon;
	Data::Shield shield;
	armR.Hold(weapon);
	armL.Hold(shield);
	Data::Melee melee;
	auto& limbs = b.UsedLimbs(melee);
	BOOST_CHECK(limbs.count(&armR) == 1);
	BOOST_CHECK(limbs.count(&armL) == 0);
	
	auto used = b.UsedWeapon(melee);
	BOOST_CHECK_EQUAL(used.first, &armR);
	BOOST_CHECK_EQUAL(used.second, &weapon);
}

BOOST_AUTO_TEST_CASE(UsedWeapon)
{
	Data::Human b;
	BOOST_CHECK(b[L"RArm"].Grip());
	BOOST_CHECK(b[L"LArm"].Grip());
	BOOST_CHECK(!b[L"Legs"].Grip());
}

BOOST_AUTO_TEST_SUITE_END()
}}  // Test