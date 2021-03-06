#pragma once

#include <set>
#include "Game/Position.h"
#include "Physics/Direction.h"
#include "Damage.h"
#include "Stats.h"
#include "Actor.h"

namespace Game
{
class Action;
class Skill;
class Location;
class Item;
class Part;

class State 
{
public:
	State();
    State(const Actor& actor);
    State(const Anatomy& anatomy, Position pos=Position(0,0,0), Physics::Direction dir=Physics::Direction::east, unsigned mp=10,
        std::set<const Armor*> armor = {}, std::set<const Weapon*> weapons = {}, Actor::Knowledge knowledge = {});

    Position position;
    Physics::Direction direction;
    unsigned mp;
    unsigned loyalty;
    Body body;
    std::set<const Armor*> worn;
    std::set<const Weapon*> carried;
    Actor::Knowledge knowledge;

    bool IsPossible(const Skill& skill, const Target& target) const;
    Damage AttackDamage(const Skill& skill) const;
    Damage AttackDamage(const Skill& skill, const Score& skillLevel) const;
	Damage Mitigation(const Part& location) const;
	bool Hurt(const Part& part, const Damage& damage, const std::wstring& description);
    Score Chance(const Skill& skill, const State& target) const;
    Score Chance(const Skill& skill) const;
	void Spent(unsigned mp);
	void Face(const Position& target);
	
    Score Strength() const;
    Score Agility() const;
    Score Constitution() const;
    Score Intelligence() const;
    Score Wisdom() const;
    Score AttributeScore(Attribute attribute) const;
	Location Origin(const Skill& skill) const;

    Score SkillLevel(const Skill& skill, const State* victim = nullptr) const;
    Score Range(const Skill& skill) const;
    void Engage(const Skill& skill);

	void Wear(const Armor& armor);
	void Wield(const Weapon& weapon);

private:
	void AutoArm();
    Bonus StrengthBonus() const;
    Bonus Encumberance() const;
    Bonus ConstitutionBonus() const;
    Bonus IntelligenceBonus() const;
    Bonus WisdomBonus() const;
    Bonus Charge() const;   // excess enchantment
    Score AttributeScore(const Part& limb, Attribute attribute) const;
    Score ArmorBonus(const Skill& skill) const;
    Score FullBodyBonus(Attribute attribute) const;
    Score FreeLimbScore(const Weapon& weapon, Attribute attribute) const;
    Score AttributeBonus(Attribute attribute) const;
};
} // ::Game