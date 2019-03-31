#include "stdafx.h"
#include "Engine/Xml.h"
#include "Engine/CSV.h"
#include "Engine/from_string.h"
#include "Engine/utils.h"
#include "Engine/Image.h"
#include "Engine/XML.h"
#include "Skill.h"
#include "Attack.h"
#include "React.h"
#include "Affect.h"
#include "Move.h"
#include "Actor.h"
#include "Target.h"
#include "Item.h"

namespace Game
{
Skill::Skill() :
	mp(0),
	range(0),
	trigger(Trigger::None),
	attribute(Attribute::None),
	resist(Attribute::None),
	engaging(true),
	offset(0),
	weapon(Type::Weapon::Style::None)
{
}

Skill::Skill(const std::wstring name, Trigger trigger, Type::Weapon::Style weapon, Direction singleTrajectory, int damage) :
	mp(2),
	range(0),
	trigger(trigger),
	name(name),
	attribute(Attribute::Strength),
	resist(Attribute::None),
	engaging(true),
	offset(0),
	weapon(weapon),
	type(std::make_shared<Melee>()),
	targeting({Targeting::Swing, Targeting::Face})
{
	static_cast<Melee&>(*type).damage = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
	trajectory.insert(singleTrajectory);
}

Skill::Skill(const std::wstring name, Attribute attribute, Targeting targeting) :
	mp(2),
	range(0),
	trigger(Trigger::Act),
	name(name),
	attribute(attribute),
	resist(Attribute::None),
	engaging(true),
	offset(0),
	weapon(Type::Weapon::Style::None),
	type(std::make_shared<Affect>()),
	targeting({targeting})
{
}
Skill::~Skill() = default;

TargetedAction* Skill::CreateAction(const Identity& actor, const Target& target, Direction trajectory, const Part* part) const
{
	if (type)
		return type->CreateAction(*this, actor, target, trajectory, part);
	else
		return nullptr;
}

Bonus Skill::GetChance(const Score& level) const
{
	if (chance.empty())
		return Bonus(100);
	else
	{
		auto index = level.Value();
		if (index == 0)
		{
			return Bonus();
		}
		else if (index > chance.size())
		{
			return Bonus(level.Description(), chance.back());
		}
		else
		{
			return Bonus(level.Description(), chance.at(index - 1));
		}
	}
}

TargetedAction* Skill::Move::CreateAction(const Skill& skill, const Identity& actor, const Target& target, Direction trajectory, const Part*) const
{
	return new ::Game::Move(actor, skill, target.GetPosition());
}

Skill::Melee::Melee() :
	attribute(Attribute::Strength)
{
}

TargetedAction* Skill::Melee::CreateAction(const Skill& skill, const Identity& actor, const Target& target, Direction trajectory, const Part* part) const
{
	return new Attack(actor, skill, dynamic_cast<const Actor&>(target), trajectory, part);
}

Bonus Skill::Melee::DamageBonus(const Score& skillScore)
{
	if (damage.empty())
		return Bonus();
	auto index = skillScore.Value();
	if (index == 0)
		return Bonus();
	else if (index > damage.size())
		return Bonus(skillScore.Description(), damage.back());
	else
		return Bonus(skillScore.Description(), damage.at(index - 1));
}

Skill::Affect::Affect(const xmlNode* node)
{
	for (auto prop = node->properties; prop; prop = prop->next)
	{
		throw std::runtime_error("Unknown Affect skill property");
	}
}

TargetedAction* Skill::Affect::CreateAction(const Skill& skill, const Identity& actor, const Target& target, Direction, const Part* part) const
{
	assert(false);	// not implemented
	return nullptr;
	// return new ::Game::Affect(actor, skill, dynamic_cast<const Actor&>(target), *part);
}

Skill::React::React(const xmlNode* node)
{
	for (auto prop = node->properties; prop; prop = prop->next)
	{
		throw std::runtime_error("Unknown Avoid skill property");
	}
}

TargetedAction* Skill::React::CreateAction(const Skill& skill, const Identity& actor, const Target& target, Direction direction, const Part*) const
{
	return new ::Game::React(actor, skill, dynamic_cast<const Action&>(target), direction);
}

bool Skill::Match(const std::wstring& category) const
{
	return categories.count(category) > 0;
}

bool Skill::Require(const Weapon* item) const
{
	if (weapon == ::Game::Type::Weapon::Style::None)
		return false;
	if (weapon == ::Game::Type::Weapon::Style::Unarmed && item == nullptr)
		return true;
	else
		return item && item->Match(weapon);
}

bool Skill::HasTargeting(Targeting check) const
{
	return targeting.count(check) != 0;
}

bool Skill::Combo(const Skill& previous) const
{
	if (trigger != Trigger::Combo)
		return false;
	return Follows(previous);
}

bool Skill::Follows(const Skill& previous) const
{
	for (const auto& follow : follows)
	{
		if (previous.categories.count(follow))
			return true;
	}
	return false;
}

bool Skill::Counter(const Skill& previous) const
{
	if (trigger != Trigger::Defend)
		return false;
	return Follows(previous);
}

unsigned Skill::Id() const
{
	static_assert(sizeof(unsigned) == sizeof(this), "Failed to use skill pointer as id");
	return unsigned(this);
}

Engine::Image Skill::Icon() const
{
	return *icon;
}

void Skill::LoadIcon(const std::wstring& fname)
{
	// TODO: unique pointer, but need move constructor for skills then
	icon = std::make_shared<Engine::Image>(fname);
}

bool Skill::IsActive() const
{
	return (trigger == Trigger::Act);
}

bool Skill::IsAttack() const
{
	return IsType<Melee>();
}

bool Skill::IsMove() const
{
	return IsType<Move>();
}

bool Skill::IsWait() const
{
	return type == nullptr;
}


Skill::Move::Move(const xmlNode* node)
{
}

Skill::Melee::Melee(const xmlNode* node)
{
	for (auto prop = node->properties; prop; prop = prop->next)
	{
		if (Engine::Xml::hasTag(prop, "damage"))
		{
			damage = Engine::from_strings<unsigned>(Engine::Xml::text(prop->children), L',');
		}
		else if (Engine::Xml::hasTag(prop, "attribute"))
		{
			// TODO
		}
		else
			throw std::runtime_error("Unknown Melee skill property");
	}
}



template<class T>
std::shared_ptr<T> MakeType(const xmlNode* node)
{
	return std::make_shared<T>(node);
}

Skill::Skill(const xmlNode* node) : 
	Skill()
{
	for (auto prop = node->properties; prop; prop = prop->next)
	{
		if (Engine::Xml::hasTag(prop, "name"))
		{
			name = Engine::Xml::text(prop->children);
			LoadIcon(std::wstring(L"Data/") + name + std::wstring(L".png"));
		}
		else if (Engine::Xml::hasTag(prop, "mp"))
		{
			auto mpStr = Engine::Xml::text(prop->children);
			mp = Engine::from_string<unsigned>(mpStr);
		}
		else if (Engine::Xml::hasTag(prop, "range"))
		{
			auto rangeStr = Engine::Xml::text(prop->children);
			range = Engine::from_string<unsigned>(rangeStr);
		}
		else if (Engine::Xml::hasTag(prop, "trajectory"))
		{
			trajectory = Engine::from_strings<Direction>(Engine::Xml::text(prop->children), L'|', Direction::map);
		}
		else if (Engine::Xml::hasTag(prop, "target"))
		{
			targeting = Engine::from_strings<Targeting>(Engine::Xml::text(prop->children), L'|',
				{
					{ L"self", Targeting::Self },
					{ L"face", Targeting::Face },
					{ L"swing", Targeting::Swing },
					{ L"center", Targeting::Center },
					{ L"intercept", Targeting::Intercept },
					{ L"aim", Targeting::Intercept },
					{ L"seek", Targeting::Seek},
					{ L"parabola", Targeting::Intercept },
				});
		}
		else if (Engine::Xml::hasTag(prop, "trigger"))
		{
			auto triggerStr = Engine::Xml::text(prop->children);
			trigger = Engine::from_string<Skill::Trigger>(triggerStr, {
				{ L"", Skill::Trigger::None },
				{ L"Act", Skill::Trigger::Act },
				{ L"Combo", Skill::Trigger::Combo },
				{ L"Prepare", Skill::Trigger::Prepare },
				{ L"Defend", Skill::Trigger::Defend},
				{ L"Opportunity", Skill::Trigger::Opportunity }
				});
		}
		else if (Engine::Xml::hasTag(prop, "effect"))
		{
			effects = Engine::from_strings<Skill::Effect>(Engine::Xml::text(prop->children), L'|', {
				{ L"", Skill::Effect::None },
				{ L"miss", Skill::Effect::Miss },
				{ L"interrupt", Skill::Effect::Interrupt },
				{ L"deflect", Skill::Effect::Deflect },
				{ L"reflect", Skill::Effect::Reflect },
				{ L"engage", Skill::Effect::Engage },
				{ L"disarm", Skill::Effect::Disarm },
				{ L"steal", Skill::Effect::Steal },
				{ L"drop", Skill::Effect::Drop },
				{ L"control", Skill::Effect::Control }
				});
		}
		else if (Engine::Xml::hasTag(prop, "category"))
		{
			auto str = Engine::Xml::wstr(prop->children->content);
			categories = Engine::SplitSet(str, L'|');
		}
		else if (Engine::Xml::hasTag(prop, "follows"))
		{
			auto str = Engine::Xml::wstr(prop->children->content);
			follows = Engine::SplitSet(str, L'|');
		}
		else if (Engine::Xml::hasTag(prop, "attribute"))
		{
			auto str = Engine::Xml::wstr(prop->children->content);
			attribute = Engine::from_string<Attribute>(str, Attributes::map);
		}
		else if (Engine::Xml::hasTag(prop, "resist"))
		{
			auto str = Engine::Xml::wstr(prop->children->content);
			resist = Engine::from_string<Attribute>(str, Attributes::map);
		}
		else if (Engine::Xml::hasTag(prop, "engaging"))
		{
			auto str = Engine::Xml::wstr(prop->children->content);
			engaging = Engine::from_string<bool>(str);
		}
		else if (Engine::Xml::hasTag(prop, "offset"))
		{
			auto str = Engine::Xml::wstr(prop->children->content);
			offset = Engine::from_string<int>(str);
		}
		else if (Engine::Xml::hasTag(prop, "weapon"))
		{
			auto str = Engine::Xml::wstr(prop->children->content);
			weapon = Engine::from_string<Type::Weapon::Style>(str, Type::Weapon::styleMap);
		}
		else if (Engine::Xml::hasTag(prop, "chance"))
		{
			chance = Engine::from_strings<int>(Engine::Xml::text(prop->children), L',');
		}
		else
		{
			throw std::runtime_error("Unknown skill property");
		}
	}
	for (xmlNode *curNode = node->children; curNode; curNode = curNode->next)
	{
		if (curNode->type == XML_COMMENT_NODE)
		{
			continue;
		}
		else if (curNode->type == XML_TEXT_NODE)
		{
			auto str = Engine::Strip(Engine::Xml::text(curNode), L"\r\n\t ");
			if (!str.empty())
				description = str;
		}
		else if (curNode->type == XML_ELEMENT_NODE)
		{
			if (Engine::Xml::hasTag(curNode, "move"))
			{
				type = MakeType<Skill::Move>(curNode);
			}
			else if (Engine::Xml::hasTag(curNode, "melee"))
			{
				type = MakeType<Skill::Melee>(curNode);
			}
			else if (Engine::Xml::hasTag(curNode, "react"))
			{
				type = MakeType<Skill::React>(curNode);
			}
			else if (Engine::Xml::hasTag(curNode, "affect"))
			{
				type = MakeType<Skill::Affect>(curNode);
			}
			else if (Engine::Xml::hasTag(curNode, "description"))
			{
			}
			else
			{
				throw std::runtime_error("Unknown skill action");
			}
		}
		else
		{
			throw std::runtime_error("Unexpected child node of skill");
		}

	}
}





}