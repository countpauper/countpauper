#pragma once

#include "Position.h"
#include "Direction.h"
#include "Damage.h"
#include "Body.h"
#include "Stats.h"
#include "Actor.h"

namespace Game
{
class Action;
class Skill;
class Anatomy;
class Item;

class State : public Target
{
public:
	State();
    State(const Actor& actor);
    State(const Body& body, Position pos=Position(0,0), Direction dir=Direction::North, unsigned mp=10,
        std::vector<const Armor*> armor = {}, std::vector<const Weapon*> weapons = {}, Actor::Knowledge knowledge = {});

    Position position;
    Direction direction;
    unsigned mp;
    unsigned loyalty;
    Body body;
    std::vector<const Armor*> worn;
    std::vector<const Weapon*> carried;
    Actor::Knowledge knowledge;

    bool IsPossible(const Skill& skill, const Target& target) const;
    Damage AttackDamage(const Skill& skill) const;
    Damage AttackDamage(const Skill& skill, const Score& skillLevel) const;
    bool Hurt(const Body::Part& part, const Damage& damage, const std::wstring& description);
    Score Chance(const Skill& skill, const State& target) const;
    Score Chance(const Skill& skill) const;
	void Spent(unsigned mp);

    Score Strength() const;
    Score Agility() const;
    Score Constitution() const;
    Score Intelligence() const;
    Score Wisdom() const;
    Score AttributeScore(Attribute attribute) const;
    Anatomy Origin(const Skill& skill, Trajectory trajectory) const;

    Score SkillLevel(const Skill& skill, const State* victim = nullptr) const;
    Score Range(const Skill& skill) const;
    void Engage(const Skill& skill);
	Position GetPosition() const override;
	std::wstring Description() const override;
private:
    Bonus StrengthBonus() const;
    Bonus Encumberance() const;
    Bonus ConstitutionBonus() const;
    Bonus IntelligenceBonus() const;
    Bonus WisdomBonus() const;
    Bonus Charge() const;   // excess enchantment
    Score AttributeScore(const Body::Part& limb, Attribute attribute) const;
    Damage Mitigation(const Body::Part& location) const;
    Score ArmorBonus(const Skill& skill) const;
    Score FullBodyBonus(Attribute attribute) const;
    Score FreeLimbScore(const Weapon& weapon, Attribute attribute) const;
    Score AttributeBonus(Attribute attribute) const;
};
} // ::Game