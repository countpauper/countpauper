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
    Anatomy a;
    d.stream >> a;
	Body b(a);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(b.Strength(), 6);
    BOOST_CHECK_EQUAL(b.Dead(), false);
    BOOST_CHECK(b.Description() == L"Healthy");
}

BOOST_AUTO_TEST_CASE(SerializeHuman)
{
    Data::Human d;
    Anatomy a;
    d.stream >> a;
	Body b(a);
    BOOST_CHECK_EQUAL(a.Length(), 5);
    BOOST_CHECK_EQUAL(b.Strength(), 10);
    BOOST_CHECK_EQUAL(b.Dead(), false);
    BOOST_CHECK(b.Description() == L"Healthy");
}

BOOST_AUTO_TEST_CASE(Copy)
{
    Data::Simple anatomy;
	Body original(anatomy);
	Body b = original;

    BOOST_CHECK_EQUAL(b.Anatomical().Length(), 3);
    BOOST_CHECK_EQUAL(b.Strength(), 6);
    BOOST_CHECK_EQUAL(b.Dead(), false);
    BOOST_CHECK(b.Description() == L"Healthy");
}

BOOST_AUTO_TEST_CASE(Hurt)
{
    Data::Simple a;
	Body b(a);
    b.Hurt(a[L"All"], Damage(Wound::Type::Sharp, Score(L"", 5)));
    BOOST_CHECK(b.Description() == L"All=Grazed");
    BOOST_CHECK_EQUAL(b.Intelligence(), 5);
    BOOST_CHECK_EQUAL(b.Strength(), 5);
}

BOOST_AUTO_TEST_CASE(Dead)
{
    Data::Simple a;
	Body b(a);
	b.Hurt(a[L"All"], Damage(Wound::Type::Blunt, Score(L"", 20)));
    BOOST_CHECK(b.Description() == L"All=Shattered");
    BOOST_CHECK_EQUAL(b.Intelligence(), 0);
    BOOST_CHECK(b.Dead());
}

BOOST_AUTO_TEST_CASE(Weapon)
{
    Data::Human a;
	Body b(a);
    auto& arm = a[L"RArm"];
    BOOST_CHECK(b.Wielded(arm)==nullptr);
    Data::Blade weapon;
    b.Grab(arm, weapon);
    BOOST_CHECK(b.Wielded(arm) == &weapon);
    b.Drop(weapon);
    BOOST_CHECK(b.Wielded(arm)==nullptr);
    BOOST_CHECK_THROW(b.Grab(a[L"Legs"], weapon), std::exception);
    auto& otherArm = a[L"LArm"];
    b.Grab(otherArm, weapon);
    b.Grab(arm, weapon);
    BOOST_CHECK(b.Wielded(arm)==&weapon);
    // TODO: how, no ptr back from arm, weapon doesn't keep. pbly should for transfering from another body 
    // Enable this for disarm/stealing 
    // BOOST_CHECK(otherArm.Held()==nullptr);
}

BOOST_AUTO_TEST_CASE(Engage)
{
    Data::Human a;
	Body b(a);
    auto& arm = a[L"RArm"];
    Data::Blade weapon;
    b.Grab(arm, weapon);
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
    Data::Human a;
    BOOST_CHECK(a[L"RArm"].Grip());
    BOOST_CHECK(a[L"LArm"].Grip());
    BOOST_CHECK(!a[L"Legs"].Grip());
}

BOOST_AUTO_TEST_CASE(Vital)
{
	Data::Human a;
    BOOST_CHECK(a[L"Head"].IsVital());
    BOOST_CHECK(a[L"Chest"].IsVital());
    BOOST_CHECK(!a[L"Belly"].IsVital());
    BOOST_CHECK(!a[L"RArm"].IsVital());
    BOOST_CHECK(!a[L"LArm"].IsVital());
    BOOST_CHECK(!a[L"Legs"].IsVital());
}

BOOST_AUTO_TEST_CASE(UseDoubleHand)
{
	Data::Human a;
	Body b(a);
	auto& arm = a[L"RArm"];
	Data::Blade weapon;
	b.Grab(arm,weapon);
	Data::Melee melee;
	auto& limbs = b.UsedLimbs(melee);
	BOOST_CHECK(limbs.count(&arm) == 1);
	BOOST_CHECK(limbs.count(&a[L"LArm"]) == 1);
	BOOST_CHECK_EQUAL(limbs.size(), 2);
	auto used = b.UsedWeapon(melee);
	BOOST_CHECK_EQUAL(used.first, &arm);
	BOOST_CHECK_EQUAL(used.second, &weapon);
}

BOOST_AUTO_TEST_CASE(UseSingleHand)
{
	Data::Human a;
	Body b(a);
	auto& armR = a[L"RArm"];
	auto& armL = a[L"LArm"];
	Data::Blade weapon;
	Data::Shield shield;
	b.Grab(armR, weapon);
	b.Grab(armL, shield);
	Data::Melee melee;
	auto& limbs = b.UsedLimbs(melee);
	BOOST_CHECK(limbs.count(&armR) == 1);
	BOOST_CHECK(limbs.count(&armL) == 0);
	BOOST_CHECK_EQUAL(limbs.size(), 1);

	auto used = b.UsedWeapon(melee);
	BOOST_CHECK_EQUAL(used.first, &armR);
	BOOST_CHECK_EQUAL(used.second, &weapon);
}

BOOST_AUTO_TEST_CASE(UseHead)
{
	Data::Human a;
	Body b(a);
	auto& head = a[L"Head"];
	Data::Buff buff;
	auto& limbs = b.UsedLimbs(buff);
	BOOST_CHECK(limbs.count(&head) == 1);
	BOOST_CHECK_EQUAL(limbs.size(), 1);

	auto used = b.UsedWeapon(buff);
	BOOST_CHECK_EQUAL(used.first, &head);
	BOOST_CHECK_EQUAL(used.second, nullptr);
}


BOOST_AUTO_TEST_CASE(UsedWeapon)
{
	// TODO?
}

BOOST_AUTO_TEST_SUITE_END()
}}  // Test