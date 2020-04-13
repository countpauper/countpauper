#include "pch.h"
#include <sstream>
#include "Game/Body.h"
#include "Game/Damage.h"
#include "Data.h"

namespace Game
{
namespace Test
{

TEST(BodyTest, SerializeSimple)
{
    Data::Simple d;
    Anatomy a;
    d.stream >> a;
	Body b(a);
    EXPECT_EQ(a.Length(), 3);
    EXPECT_EQ(b.Strength(), 6);
    EXPECT_EQ(b.Dead(), false);
    EXPECT_EQ(b.Description(), L"Healthy");
}

TEST(BodyTest, SerializeHuman)
{
    Data::Human d;
    Anatomy a;
    d.stream >> a;
	Body b(a);
    EXPECT_EQ(a.Length(), 5);
    EXPECT_EQ(b.Strength(), 10);
    EXPECT_EQ(b.Dead(), false);
    EXPECT_EQ(b.Description(), L"Healthy");
}

TEST(BodyTest, Copy)
{
    Data::Simple anatomy;
	Body original(anatomy);
	Body b = original;

    EXPECT_EQ(b.Anatomical().Length(), 3);
    EXPECT_EQ(b.Strength(), 6);
    EXPECT_EQ(b.Dead(), false);
    EXPECT_EQ(b.Description(), L"Healthy");
}

TEST(BodyTest, Hurt)
{
    Data::Simple a;
	Body b(a);
    b.Hurt(a[L"All"], Damage(Wound::Type::Sharp, Score(L"", 5)));
    EXPECT_EQ(b.Description(), L"All=Grazed");
    EXPECT_EQ(b.Intelligence(), 5);
    EXPECT_EQ(b.Strength(), 5);
}

TEST(BodyTest, Dead)
{
    Data::Simple a;
	Body b(a);
	b.Hurt(a[L"All"], Damage(Wound::Type::Blunt, Score(L"", 20)));
    EXPECT_EQ(b.Description(), L"All=Shattered");
    EXPECT_EQ(b.Intelligence(), 0);
    EXPECT_TRUE(b.Dead());
}

TEST(BodyTest, Weapon)
{
    Data::Human a;
	Body b(a);
    auto& arm = a[L"RArm"];
    EXPECT_EQ(b.Wielded(arm), nullptr);
    Data::Blade weapon;
    b.Grab(arm, weapon);
    EXPECT_TRUE(b.Wielded(arm) == &weapon);
    b.Drop(weapon);
    EXPECT_EQ(b.Wielded(arm), nullptr);
    EXPECT_THROW(b.Grab(a[L"Legs"], weapon), std::exception);
    auto& otherArm = a[L"LArm"];
    b.Grab(otherArm, weapon);
    b.Grab(arm, weapon);
    EXPECT_TRUE(b.Wielded(arm)==&weapon);
    // TODO: how, no ptr back from arm, weapon doesn't keep. pbly should for transfering from another body 
    // Enable this for disarm/stealing 
    // EXPECT_TRUE(otherArm.Held()==nullptr);
}

TEST(BodyTest, Engage)
{
    Data::Human a;
	Body b(a);
    auto& arm = a[L"RArm"];
    Data::Blade weapon;
    b.Grab(arm, weapon);
    Data::Melee first;
    Data::Combo combo;
    EXPECT_TRUE(b.Ready(first));
    b.Engage(first);
    EXPECT_FALSE(b.Ready(first));
    EXPECT_TRUE(b.Ready(combo));
    b.Disengage();
    EXPECT_TRUE(b.Ready(first));
}


TEST(BodyTest, Grip)
{
    Data::Human a;
    EXPECT_TRUE(a[L"RArm"].Grip());
    EXPECT_TRUE(a[L"LArm"].Grip());
    EXPECT_FALSE(a[L"Legs"].Grip());
}

TEST(BodyTest, Vital)
{
	Data::Human a;
    EXPECT_TRUE(a[L"Head"].IsVital());
    EXPECT_TRUE(a[L"Chest"].IsVital());
    EXPECT_FALSE(a[L"Belly"].IsVital());
    EXPECT_FALSE(a[L"RArm"].IsVital());
    EXPECT_FALSE(a[L"LArm"].IsVital());
    EXPECT_FALSE(a[L"Legs"].IsVital());
}

TEST(BodyTest, UseDoubleHand)
{
	Data::Human a;
	Body b(a);
	auto& arm = a[L"RArm"];
	Data::Blade weapon;
	b.Grab(arm,weapon);
	Data::Melee melee;
	auto& limbs = b.UsedLimbs(melee);
	EXPECT_EQ(limbs.count(&arm), 1);
    EXPECT_EQ(limbs.count(&a[L"LArm"]), 1);
	EXPECT_EQ(limbs.size(), 2);
	auto used = b.UsedWeapon(melee);
	EXPECT_EQ(used.first, &arm);
	EXPECT_EQ(used.second, &weapon);
}

TEST(BodyTest, UseSingleHand)
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
    EXPECT_EQ(limbs.count(&armR), 1);
    EXPECT_EQ(limbs.count(&armL), 0);
	EXPECT_EQ(limbs.size(), 1);

	auto used = b.UsedWeapon(melee);
	EXPECT_EQ(used.first, &armR);
	EXPECT_EQ(used.second, &weapon);
}

TEST(BodyTest, UseHead)
{
	Data::Human a;
	Body b(a);
	auto& head = a[L"Head"];
	Data::Buff buff;
	auto& limbs = b.UsedLimbs(buff);
    EXPECT_EQ(limbs.count(&head), 1);
	EXPECT_EQ(limbs.size(), 1);

	auto used = b.UsedWeapon(buff);
	EXPECT_EQ(used.first, &head);
	EXPECT_EQ(used.second, nullptr);
}


TEST(BodyTest, UsedWeapon)
{
	// TODO?
}

}}  // Test