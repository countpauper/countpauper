#pragma once
#include <memory>
#include <set>
#include <vector>
#include "Engine/Identification.h"
#include "Item.h"
#include "Physics/Direction.h"
#include "Targeting.h"
#include "Stats.h"


extern "C" {
struct _xmlNode;
typedef struct _xmlNode xmlNode;
}

namespace Engine { class Image; }
namespace Game
{
class TargetedAction;
class Identity;
class Target;
class Part;

class Skill : public Engine::Identification
{
public:
	Skill();
	enum class Trigger { None = 0, Act, Combo, Prepare, Defend, Opportunity };
	Skill(const std::wstring name, Trigger trigger, Type::Weapon::Style weapon, Physics::Direction trajectory, int damage);
	Skill(const std::wstring name, Attribute attribute, Targeting targeting);
	Skill(const xmlNode* node);
	virtual ~Skill();

	TargetedAction* CreateAction(const Identity& actor, const Target& target, Physics::Direction trajectory, const Part* part) const;
	Bonus GetChance(const Score& level) const;

	bool Match(const std::wstring& category) const;
	bool Require(const Weapon* item) const;
	bool HasTargeting(Targeting check) const;
	bool Combo(const Skill& previous) const;
	bool Counter(const Skill& previous) const;
	bool Follows(const Skill& previous) const;

	Engine::Image Icon() const;
	void LoadIcon(const std::wstring& fname);

	bool IsActive() const;
	bool IsAttack() const;
	bool IsMove() const;
	bool IsWait() const;

	std::wstring name;
	std::wstring description;
	unsigned mp;
	unsigned range;

	Trigger trigger;
	using Category = std::wstring;
	using Categories = std::set<Category>;
	Categories categories;
	Categories follows;
	Attribute attribute;
	Attribute resist;
	bool engaging;
	int offset;
	Type::Weapon::Style weapon;
	using Prerequisite = std::pair<Skill*, unsigned>;
	std::vector<Prerequisite> prerequisites;
	std::vector<int> chance;
	std::set<Physics::Direction> trajectory;
	std::set<Targeting> targeting;

	enum class Effect {
		None,
		Miss,       // no effect on actor
		Interrupt,  // stop action
		Deflect,    // other target within range
		Reflect,    // effect attacker
		Engage,     // control target limb with this skill
		Disarm,     // remove held item
		Steal,      // remove targeted item and give to attacker
		Drop,       // remove targeted item and put it on the floor as an object
		Control,    // control targeted limb
	};
	using Effects = std::set<Effect>;
	Effects effects;

	class Act
	{
	public:
		virtual ~Act() = default;
		virtual TargetedAction* CreateAction(const Skill& skill, const Identity& actor, const Target& target, Physics::Direction trajectory, const Part* part) const = 0;
	};
	class Move : public Act
	{
	public:
		Move() = default;
		Move(const xmlNode* node);
		TargetedAction* CreateAction(const Skill& skill, const Identity& actor, const Target& target, Physics::Direction trajectory, const Part*) const override;
	};
	class Melee : public Act
	{
	public:
		Melee();
		Melee(const xmlNode* node);
		std::vector<unsigned> damage;
		Attribute attribute;
		Bonus DamageBonus(const Score& skillScore);
		TargetedAction* CreateAction(const Skill& skill, const Identity& actor, const Target& target, Physics::Direction trajectory, const Part* part) const override;
	};
	class Affect : public Act
	{
	public:
		Affect() = default;
		Affect(const xmlNode* node);
		TargetedAction* CreateAction(const Skill& skill, const Identity& actor, const Target& target, Physics::Direction trajectory, const Part* part) const override;
	};
	class React : public Act
	{
	public:
		React() = default;
		React(const xmlNode* node);
		TargetedAction* CreateAction(const Skill& skill, const Identity& actor, const Target& target, Physics::Direction trajectory, const Part*) const override;
	};
	std::shared_ptr<Act> type; // todo: unique ptr and copy/clone or move
private:

	template<class T>
	bool IsType() const
	{
		return dynamic_cast<T*>(type.get()) != nullptr;
	}
	std::shared_ptr<Engine::Image> icon;
};


}