#include "pch.h"
#include "Game/State.h"
#include "Data.h"

namespace Game
{
namespace Test
{

TEST(State, BodyStatistics)
{
    Data::Simple a;
	Body b(a);
    State s(a);
    EXPECT_EQ(s.Strength(), b.Strength());
    EXPECT_EQ(s.Agility(), b.Agility());
    EXPECT_EQ(s.Constitution(), b.Constitution());
    EXPECT_EQ(s.Intelligence(), b.Intelligence());
    EXPECT_EQ(s.Wisdom(), b.Wisdom());

    EXPECT_EQ(s.AttributeScore(Attribute::Wisdom), b.Wisdom());
}

TEST(State, NoSkill)
{
    Data::Simple a;
    Data::Blade weapon;
    Data::Melee skill;
    State s(a, Position(), Direction(), 10, {}, { &weapon }, {});
    EXPECT_EQ(s.SkillLevel(skill), 0);
    Data::Victim v;
    EXPECT_FALSE(s.IsPossible(skill, v));
}

TEST(State, NoWeapon)
{
    Data::Simple a;
	Body b(a);
    Data::Melee skill;
    State s(a, Position(), Direction(), 10, {}, {}, { &skill });
    auto level = s.SkillLevel(skill);
    EXPECT_EQ(level, b.Strength());
    Data::Victim v;
    EXPECT_FALSE(s.IsPossible(skill, v));
}

TEST(State, UseWeapon)
{
    Data::Simple a;
	Body b(a);
    Data::Blade weapon;
    Data::Melee skill;
    State s(a, Position(), Direction(), 10, {}, { &weapon }, { &skill });
    Data::Victim v;
    EXPECT_TRUE(s.IsPossible(skill, v));

    Damage d = s.AttackDamage(skill);
    EXPECT_EQ(d.Sharp(), weapon.Damage().Sharp().Value() + static_cast<Skill::Melee&>(*skill.type).DamageBonus(b.Strength()).value);
}

TEST(State, DoubleHand)
{
    Data::Human a;
    Data::Blade weapon;
	Data::Melee skill;
    State s(a, Position(), Direction(), 10, {}, { &weapon }, { &skill });
    Data::Victim v;
    EXPECT_TRUE(s.IsPossible(skill, v));

    auto skillLevel = s.SkillLevel(skill, &v);
    EXPECT_EQ(skillLevel, s.body.Strength());
    Damage d = s.AttackDamage(skill, skillLevel);
    EXPECT_EQ(d.Sharp(), weapon.Damage().Sharp() + static_cast<Skill::Melee&>(*skill.type).DamageBonus(s.body.Strength()));
}

TEST(State, SingleHand)
{
    Data::Human a;
    Data::Blade weapon;
    Data::Shield shield;
    Data::Melee skill;
    State s(a, Position(), Direction(), 10, {}, { }, { &skill });
	s.Wield(weapon);
	s.Wield(shield);
	ASSERT_EQ(s.body.Wielded(a[L"RArm"]), &weapon);
    ASSERT_EQ(s.body.Wielded(a[L"LArm"]), &shield);

	auto rightArmStrength = a[L"RArm"].AttributeScore(Attribute::Strength);
	Data::Victim v;
	EXPECT_TRUE(s.IsPossible(skill, v));
    auto skillLevel = s.SkillLevel(skill, &v);
    EXPECT_EQ(skillLevel, rightArmStrength);
    Damage d = s.AttackDamage(skill, skillLevel);
    EXPECT_EQ(d.Sharp(), weapon.Damage().Sharp() + static_cast<Skill::Melee&>(*skill.type).DamageBonus(rightArmStrength));
}

TEST(State, HeavyWeapon)
{
    Data::Knight s;
    s.weapon.mat.name = L"Led";
    s.weapon.mat.requirement.strength = s.Strength().Value() + 3;
    s.weapon.mat.load.weight = 80;
    EXPECT_EQ(s.Agility(), s.body.Agility().Value() - 6);  // (strength * 5 - 80 )/5 = -6

    Data::Victim v;
    EXPECT_TRUE(s.IsPossible(s.melee, v));
    EXPECT_EQ(s.SkillLevel(s.melee, &v), s.body.Strength().Value() - 3);
}

TEST(State, ChargedWeapon)
{
	Data::Mage s;
	s.weapon.mod.requirement.intelligence = s.Intelligence().Value() + 2;
	s.weapon.mod.prefix = L"Astral";
	s.weapon.mod.load.enchantment = 75;
	EXPECT_EQ(s.Intelligence(), s.body.Intelligence().Value() - 5);  // (wis * 5 - 75)/5 = -5

    EXPECT_TRUE(s.IsPossible(s.buff, s));
	EXPECT_EQ(s.SkillLevel(s.buff, nullptr), 5);
}

TEST(State, Mitigation)
{
    Data::Harnass armor;
    armor.mod.load.enchantment = 70;
    Data::Knight s;
    s.Wear(armor);
    EXPECT_EQ(s.Intelligence(), s.body.Intelligence().Value() - 4);
    Location hitLocation(Plane::front, 3, 1);
	auto& part = s.body.Anatomical()[L"Chest"];
	ASSERT_TRUE(part.IsVital());
    EXPECT_FALSE(s.Hurt(part, Damage(Wound::Type::Sharp, Score(L"", 1)) - s.Mitigation(part), L"Small wound"));
    EXPECT_FALSE(s.body.IsHurt());
    EXPECT_FALSE(s.Hurt(part, Damage(Wound::Type::Sharp, Score(L"", 4)) - s.Mitigation(part), L"Medium wound"));
    EXPECT_FALSE(s.body.IsHurt());
	EXPECT_TRUE(s.Hurt(part, Damage(Wound::Type::Sharp, Score(L"", 5)) - s.Mitigation(part), L"Big wound"));
    EXPECT_TRUE(s.body.IsHurt());
    EXPECT_TRUE(s.Hurt(part, Damage(Wound::Type::Sharp, Score(L"", 25)) - s.Mitigation(part), L"Deadly wound"));
    EXPECT_TRUE(s.body.Dead());
    EXPECT_TRUE(s.direction.IsProne());
}

TEST(State, Range)
{
    Data::Knight s;
    EXPECT_EQ(s.Range(s.melee), s.melee.range + s.body.Anatomical()[L"RArm"].Length() + s.weapon.Length());
}

TEST(State, Engage)
{
    Data::Knight s;
    Data::Victim v;
    ASSERT_TRUE(s.IsPossible(s.melee, v));
    s.Engage(s.melee);
	EXPECT_FALSE(s.IsPossible(s.melee, v));
    EXPECT_TRUE(s.IsPossible(s.combo, v));
}

TEST(State, SkillPossibleMP)
{
	Data::Knight s;
	Data::Victim v;
    ASSERT_TRUE(s.IsPossible(s.melee, v));
	s.mp = s.melee.mp-1;
    EXPECT_FALSE(s.IsPossible(s.melee, v));
}

TEST(State, SkillPossibleWielded)
{
	Data::Knight s;
	Data::Victim v;
    ASSERT_TRUE(s.IsPossible(s.melee, v));
	s.body.Drop(s.weapon);
    EXPECT_FALSE(s.IsPossible(s.melee, v));
}

TEST(State, SkillPossibleRange)
{
	Data::Knight s;
	Data::Victim v;
    ASSERT_TRUE(s.IsPossible(s.melee, v));
	// 3 El can hit exactly 1 horizontal/vertical square
	ASSERT_EQ(s.melee.range + s.weapon.Length() + (*s.body.Anatomical().Grip().begin())->Length(), 3);
	v.position.x = s.position.x + 1;
	EXPECT_TRUE(s.IsPossible(s.melee, v));
	v.position.y = s.position.y + 1;
	EXPECT_FALSE(s.IsPossible(s.melee, v));
}

TEST(State, Face)
{
	Data::Knight s;
	s.Face(s.position + Position(1, 0, 0));
	EXPECT_EQ(s.direction, Direction::east);
	s.Face(s.position + Position(0, -1, 0));
	EXPECT_EQ(s.direction, Direction::south);
	s.Face(s.position);
	EXPECT_EQ(s.direction, Direction::south);
}


}
}  // Game::Test
