#include <boost/test/unit_test.hpp>
#include <sstream>
#include "Body.h"
#include "Damage.h"
#include "Item.h"
#include "Skills.h"

namespace Game
{
namespace Test
{
BOOST_AUTO_TEST_SUITE(BodyTest);

class Data
{
public:
    Data() :
        simpleStream(
        L"1\n"
        L"Body All[0-2] Full All 6 ,,,,\n"
        ),
        humanStream(
        L"6 \n"
        L"Head All[4] Head Int 10 ,,,, \n"
        L"Chest Sagittal[3] Chest Wis 10 ,,,,\n"
        L"Belly Sagittal[2] Belly Con 10 ,,,,\n"
        L"RArm Right[2-3] Arms Str 6 ,,,,\n"
        L"LArm Left[2-3] Arms Str 4 ,,,,\n"
        L"Legs Around[0-1] Legs Agi 10 ,,,,\n"
        ),
        weaponType(Type::Weapon::Style::Blade, 1, Damage(Wound::Type::Sharp,Score(L"",1))),
        weapon(weaponType, mat, mod),
        melee(L"Melee", Skill::Trigger::Act, Type::Weapon::Style::Blade, Trajectory::Straight, 1),
        combo(L"Combo", Skill::Trigger::Combo, Type::Weapon::Style::Blade, Trajectory::Straight, 1)
    {
        simple.Add(Body::Part(L"All", Anatomy(Plane::All, 0, 3), Slot::Full, Stats(6, 6, 6, 6, 6), Damage()));

        human.Add(Body::Part(L"Head", Anatomy(Plane::All, 4, 1), Slot::Head, Stats(Attribute::Intelligence, 10), Damage()));
        human.Add(Body::Part(L"Chest", Anatomy(Plane::Sagittal, 4, 1), Slot::Chest, Stats(Attribute::Wisdom, 10), Damage()));
        human.Add(Body::Part(L"Belly", Anatomy(Plane::Sagittal, 4, 1), Slot::Belly, Stats(Attribute::Constitution, 10), Damage()));
        human.Add(Body::Part(L"RArm", Anatomy(Plane::Right, 4, 1), Slot::Arms, Stats(Attribute::Strength, 6), Damage()));
        human.Add(Body::Part(L"LArm", Anatomy(Plane::Left, 4, 1), Slot::Arms, Stats(Attribute::Strength, 4), Damage()));
        human.Add(Body::Part(L"Legs", Anatomy(Plane::Around, 4, 1), Slot::Legs, Stats(Attribute::Agility, 10), Damage()));
        melee.categories.insert(L"1");
        combo.follows.insert(L"1");
    }
    std::wstringstream simpleStream;
    std::wstringstream humanStream;
    Body simple;
    Body human;
    Type::Weapon weaponType;
    Type::Weapon::Material mat;
    Type::Weapon::Modifier mod;
    Weapon weapon;
    Skill melee;
    Skill combo;
};

BOOST_AUTO_TEST_CASE(SerializeSimple)
{
    Data d;
    Body b;
    d.simpleStream >> b;
    BOOST_CHECK_EQUAL(b.Length(), 3);
    BOOST_CHECK_EQUAL(b.Strength().Value(), 6);
    BOOST_CHECK_EQUAL(b.Dead(), false);
    BOOST_CHECK(b.Description() == L"Healthy");
}

BOOST_AUTO_TEST_CASE(SerializeHuman)
{
    Data d;
    Body b;
    d.humanStream >> b;
    BOOST_CHECK_EQUAL(b.Length(), 5);
    BOOST_CHECK_EQUAL(b.Strength().Value(), 10);
    BOOST_CHECK_EQUAL(b.Dead(), false);
    BOOST_CHECK(b.Description() == L"Healthy");
}

BOOST_AUTO_TEST_CASE(Copy)
{
    Data d;
    auto b = d.simple;
    BOOST_CHECK_EQUAL(b.Length(), 3);
    BOOST_CHECK_EQUAL(b.Strength().Value(), 6);
    BOOST_CHECK_EQUAL(b.Dead(), false);
    BOOST_CHECK(b.Description() == L"Healthy");
}

BOOST_AUTO_TEST_CASE(Hurt)
{
    Data d;
    auto b = d.simple;
    b[L"All"].Hurt(Damage(Wound::Type::Sharp, Score(L"", 5)));
    BOOST_CHECK(b.Description() == L"All=Grazed");
    BOOST_CHECK_EQUAL(b.Intelligence().Value(), 5);
    BOOST_CHECK_EQUAL(b.Strength().Value(), 5);
}

BOOST_AUTO_TEST_CASE(Dead)
{
    Data d;
    auto b = d.simple;
    b[L"All"].Hurt(Damage(Wound::Type::Blunt, Score(L"", 20)));
    BOOST_CHECK(b.Description() == L"All=Shattered");
    BOOST_CHECK_EQUAL(b.Intelligence().Value(), 0);
    BOOST_CHECK(b.Dead());
}

BOOST_AUTO_TEST_CASE(Weapon)
{
    Data d;
    auto b = d.human;
    auto arm = b[L"RArm"];
    BOOST_CHECK(arm.Held() == nullptr);
    arm.Hold(d.weapon);
    BOOST_CHECK(arm.Held() == &d.weapon);
    arm.Drop();
    BOOST_CHECK(arm.Held() == nullptr);
    BOOST_CHECK_THROW(b[L"Legs"].Hold(d.weapon), std::exception);
}

BOOST_AUTO_TEST_CASE(Engage)
{
    Data d;
    auto b = d.human;
    auto arm = b[L"RArm"];
    arm.Hold(d.weapon);
    BOOST_CHECK(b.Ready(d.melee));
    b.Engage(d.melee);
    BOOST_CHECK(!b.Ready(d.melee));
    BOOST_CHECK(b.Ready(d.combo));
    b.Disengage();
    BOOST_CHECK(b.Ready(d.melee));
}


BOOST_AUTO_TEST_SUITE_END()
}}  // Test