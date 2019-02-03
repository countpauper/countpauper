#include <boost/test/unit_test.hpp>
#include "Game/State.h"
#include "Data.h"

namespace Game
{
namespace Test
{

BOOST_AUTO_TEST_SUITE(StateTest);

BOOST_AUTO_TEST_CASE(BodyStatistics)
{
    Data::Simple b;
    State s(b);
    BOOST_CHECK_EQUAL(s.Strength(), b.Strength());
    BOOST_CHECK_EQUAL(s.Agility(), b.Agility());
    BOOST_CHECK_EQUAL(s.Constitution(), b.Constitution());
    BOOST_CHECK_EQUAL(s.Intelligence(), b.Intelligence());
    BOOST_CHECK_EQUAL(s.Wisdom(), b.Wisdom());

    BOOST_CHECK_EQUAL(s.AttributeScore(Attribute::Wisdom), b.Wisdom());
}

BOOST_AUTO_TEST_CASE(NoSkill)
{
    Data::Simple a;
    Data::Blade weapon;
    Data::Melee skill;
    State s(a, Position(), Direction(), 10, {}, { &weapon }, {});
    BOOST_CHECK_EQUAL(s.SkillLevel(skill), 0);
    Data::Victim v;
    BOOST_CHECK(!s.IsPossible(skill, v));
}

BOOST_AUTO_TEST_CASE(NoWeapon)
{
    Data::Simple b;
    Data::Melee skill;
    State s(b, Position(), Direction(), 10, {}, {}, { &skill });
    auto level = s.SkillLevel(skill);
    BOOST_CHECK_EQUAL(level, b.Strength());
    Data::Victim v;
    BOOST_CHECK(!s.IsPossible(skill, v));
}

BOOST_AUTO_TEST_CASE(UseWeapon)
{
    Data::Simple b;
    Data::Blade weapon;
    b[L"All"].Hold(weapon);
    Data::Melee skill;
    State s(b, Position(), Direction(), 10, {}, { &weapon }, { &skill });
    Data::Victim v;
    BOOST_CHECK(s.IsPossible(skill, v));

    Damage d = s.AttackDamage(skill);
    BOOST_CHECK_EQUAL(d.Sharp(), weapon.Damage().Sharp().Value() + static_cast<Skill::Melee&>(*skill.type).DamageBonus(b.Strength()).value);
}

BOOST_AUTO_TEST_CASE(DoubleHand)
{
    Data::Human b;
    Data::Blade weapon;
    b[L"RArm"].Hold(weapon);
    Data::Melee skill;
    State s(b, Position(), Direction(), 10, {}, { &weapon }, { &skill });
    Data::Simple vb;
    State v(vb);
    BOOST_CHECK(s.IsPossible(skill, v));

    auto skillLevel = s.SkillLevel(skill, &v);
    BOOST_CHECK_EQUAL(skillLevel, b.Strength());
    Damage d = s.AttackDamage(skill, skillLevel);
    BOOST_CHECK_EQUAL(d.Sharp(), weapon.Damage().Sharp() + static_cast<Skill::Melee&>(*skill.type).DamageBonus(b.Strength()));
}

BOOST_AUTO_TEST_CASE(SingleHand)
{
    Data::Human b;
    Data::Blade weapon;
    b[L"RArm"].Hold(weapon);
    auto rightArmStrength = b[L"RArm"].AttributeScore(Attribute::Strength);
    Data::Shield shield;
    b[L"LArm"].Hold(shield);
    Data::Melee skill;
    State s(b, Position(), Direction(), 10, {}, { &weapon }, { &skill });
    Data::Simple vb;
    State v(vb);
    BOOST_CHECK(s.IsPossible(skill, v));
    auto skillLevel = s.SkillLevel(skill, &v);
    BOOST_CHECK_EQUAL(skillLevel, rightArmStrength);
    Damage d = s.AttackDamage(skill, skillLevel);
    BOOST_CHECK_EQUAL(d.Sharp(), weapon.Damage().Sharp() + static_cast<Skill::Melee&>(*skill.type).DamageBonus(rightArmStrength));
}

BOOST_AUTO_TEST_CASE(HeavyWeapon)
{
    Data::Attacker s;
    s.weapon.mat.name = L"Led";
    s.weapon.mat.requirement.strength = s.Strength().Value() + 3;
    s.weapon.mat.load.weight = 80;
    BOOST_CHECK_EQUAL(s.Agility(), s.body.Agility().Value() - 6);  // (strength * 5 - 80 )/5 = -6

    Data::Victim v;
    BOOST_CHECK(s.IsPossible(s.skill, v));
    BOOST_CHECK_EQUAL(s.SkillLevel(s.skill, &v), s.body.Strength().Value() - 3);
}

BOOST_AUTO_TEST_CASE(Mitigation)
{
    Data::Harnass armor;
    armor.mod.load.enchantment = 70;
    Data::Attacker s;
    s.worn.push_back(&armor);
    BOOST_CHECK_EQUAL(s.Intelligence(), s.body.Intelligence().Value() - 4);
    Anatomy hitLocation(Plane::Front, 3, 1);
    s.Hurt(hitLocation, Damage(Wound::Type::Sharp, Score(L"", 1)), L"Small wound");
    BOOST_CHECK(!s.body.Get(hitLocation)->IsHurt());
    s.Hurt(hitLocation, Damage(Wound::Type::Sharp, Score(L"", 4)), L"Medium wound");
    BOOST_CHECK(!s.body.Get(hitLocation)->IsHurt());
    s.Hurt(hitLocation, Damage(Wound::Type::Sharp, Score(L"", 5)), L"Big wound");
    BOOST_CHECK(s.body.Get(hitLocation)->IsHurt());
    s.Hurt(hitLocation, Damage(Wound::Type::Sharp, Score(L"", 25)), L"Deadly wound");
    BOOST_CHECK(s.body.Dead());
    BOOST_CHECK(s.direction.Prone());
}

BOOST_AUTO_TEST_CASE(Range)
{
    Data::Attacker s;
    BOOST_CHECK_EQUAL(s.Range(s.skill), s.skill.range + s.body[L"RArm"].Length() + s.weapon.Length());
}

BOOST_AUTO_TEST_CASE(Engage)
{
    Data::Attacker s;
    Data::Victim v;
    BOOST_REQUIRE(s.IsPossible(s.skill, v));
    s.Engage(s.skill);
    BOOST_CHECK(!s.IsPossible(s.skill, v));

}
BOOST_AUTO_TEST_SUITE_END()
}
}  // Game::Test
