#include "stdafx.h"
#include "Body.h"
#include "Score.h"
#include "Skills.h"
#include "Body.h"
#include "Damage.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <iterator>

namespace Game
{
    Part::Part() :
        slot(Slot::Nothing)
    {
    }

    Part::Part(std::wstring name, Location location, Slot slot, const Stats& stats) :
        name(name),
		location(location),
        slot(slot),
        stats(stats)
    {
    }

	Part::Part(Part&& other) :
		Part(other.name, other.location, other.slot, other.stats)
	{
		other.name.clear();
		other.location = Location();
		other.slot = Slot::Nothing;
		other.stats = Stats();
	}



	std::wstring Part::Name() const
	{
		return name;
	}
	std::wstring Part::Description() const
	{
		return name;
	}

	Slot Part::GetSlot() const
	{
		return slot;
	}

	unsigned Part::Height() const
	{
		return location.position + location.size;
	}

	unsigned Part::Length() const
	{
		return location.size;
	}

	bool Part::Match(Location match) const
    {
        return location.Contains(match);
    }

	Score Part::AttributeScore(Attribute attribute) const
	{
		return Score(name, stats[attribute]);
	}

    bool Part::IsVital() const
    {
        // TODO: flag
        return Contributes(Attribute::Intelligence) || 
            Contributes(Attribute::Wisdom);
    }

	Location Part::GetLocation() const
	{
		return location;
	}
    bool Part::Grip() const
    {
        // TODO: flag
        return Contributes(Attribute::Strength);
    }

    bool Part::Contributes(Attribute attribute) const
    {
        return stats[attribute] > 0;
    }


    const Part* Anatomy::At(const Location& location) const
    {
        for (auto& part : parts)
        {
            if (part.Match(location))
            {
                return &part;
            }
        }
        return nullptr;
    }

    const Part* Anatomy::Get(const std::wstring& name) const
    {
        for (auto& part : parts)
        {
            if (part.Name() == name)
            {
                return &part;
            }
        }
        return nullptr;
    }

	Part& Anatomy::operator[](const std::wstring& name)
    {
		auto part = Get(name);
		if (!part)
			throw std::out_of_range("Unknown anatomy part");
		return const_cast<Part&>(*part);
    }

	const Part& Anatomy::operator[](const std::wstring& name) const
	{
		auto part = Get(name);
		if (!part)
			throw std::out_of_range("Unknown anatomy part");
		return *part;
	}

    std::set<const Part*> Anatomy::Grip() const
    {
        std::set<const Part*> result;
        for (auto& part : parts)
        {
            if (part.Grip())
                result.emplace(&part);
        }
        return result;
    }


    std::set<const Part*> Anatomy::KineticChain(const Part& origin) const
    {
        // Doesn't actually connect the whole chain, just adds a (first) agility body part
        std::set<const Part*> result;
        result.emplace(&origin);
        for (auto& part : parts)
        {
            // TODO: depends on position of actor, flying, swimming, standing
            if (part.Contributes(Attribute::Agility))
            {
                result.emplace(&part);
                break;
            }
        }
        return result;
    }

    unsigned Anatomy::Length() const
    {
        return std::accumulate(parts.begin(), parts.end(), 0U, [](unsigned max, const decltype(parts)::value_type& part)
        {
            return std::max(max, part.Height());
        });
    }

    Damage Anatomy::InnateDamage() const
    {
        // TODO: configure limb used and use its current "strength" for bite/tail attacks  
        return Damage(Wound::Type::Blunt, Score(L"Innate",1));
    }

    void Anatomy::Add(Part&& p)
    {
        parts.emplace_back(std::move(p));
    }

    std::wistream& operator>>(std::wistream& s, Part& part)
    {
        Attributes attributes;
        Stats::Score score;
        s >> part.name >> part.location >> part.slot >> attributes >> score;
        part.stats = Stats(attributes, score);

        return s;
    }

    std::wistream& operator>>(std::wistream& s, Anatomy& anatomy)
    {
        unsigned parts;
        s >> parts;
		anatomy.parts.resize(parts);
        for (unsigned i = 0; i < parts; ++i)
        {
            s >> anatomy.parts[i];
        }
        return s;
    }


	Body::Bit::Bit(const Part& part, const Damage& health) :
		part(part),
		health(health),
		engagement(nullptr),
		held(nullptr),
		control(nullptr)
	{
	}

	Body::Bit& Body::Bit::operator=(const Body::Bit& other)
	{
		if (!Match(other.part))
			throw std::invalid_argument("Incompatible bits");
		health = other.health;
		engagement = other.engagement;
		held = other.held;
		control = other.control;
		return *this;
	}

	bool Body::Bit::Match(const Part& match) const
	{
		return &part== &match;
	}

	std::wstring Body::Bit::Description() const
	{
		return part.Description()+L"=" + health.StateDescription();
	}



	bool Body::Bit::Disabled() const
	{
		return health.Disabled();
	}


	void Body::Bit::Hurt(const Damage& damage)
	{
		health += damage;
	}

	Score Body::Bit::AttributeScore(Attribute attribute) const
	{
		auto base = part.AttributeScore(attribute);
		if (base.Value() != 0)
			return base + Bonus(health.StateDescription(), health.StatPenalty());
		else
			return Score();
	}

	bool Body::Bit::IsHurt() const
	{
		return health.Hurt();
	}


	void Body::Bit::Engage(const Skill& skill)
	{
		engagement = &skill;
	}

	void Body::Bit::Disengage()
	{
		engagement = nullptr;
	}

	void Body::Bit::Hold(const Weapon& item)
	{
		if (!part.Grip())
			throw std::runtime_error("Body part can't grip item");
		held = &item;
	}

	const Weapon* Body::Bit::Held() const
	{
		return held;
	}

	void Body::Bit::Drop()
	{
		held = nullptr;
	}

	bool Body::Bit::IsAvailable(const Skill& skill) const
	{
		if (control)
			return false;
		if ((engagement) && (!skill.Combo(*engagement)))
			return false;
		if (part.Grip())
		{
			if (skill.Require(held))
				return true;
			else if (!held)
				return true;
			else
				return false;
		}
		else
			return true;
	}

	bool Body::Bit::IsUsed(const Skill& skill) const
	{
		if (part.Grip())
		{
			if ((skill.Require(held)) && (part.Contributes(skill.attribute)))
				return true;	// main hand
			else if ((held == nullptr) && (part.Contributes(skill.attribute)))
				return true;	// double hand
			else if ((held != nullptr) && (skill.Require(held)))
				return true;	// component
			else
				return false;
		}
		else
			return part.Contributes(skill.attribute);
	}


	Body::Body() :
		anatomy(nullptr)
	{
	}

	Body::Body(const Anatomy& _anatomy) :
		anatomy(&_anatomy)
	{
		Initialize();
	}

	void Body::Initialize()
	{
		bits.clear();
		for (const auto& part : anatomy->parts)
		{
			bits.emplace_back(Bit(part, Damage()));
		}
	}

	const Anatomy& Body::Anatomical() const
	{
		static Anatomy spirit;
		if (anatomy)
			return *anatomy;
		else
			return spirit;
	}
	std::wstring Body::Description() const
	{
		std::wstring result;
		bool fine = true;
		for (auto& bit: bits)
		{
			if (bit.IsHurt())
			{
				if (!fine)
					result += L",";
				result += bit.Description();
				fine = false;
			}
		}
		if (fine)
			return L"Healthy";
		else
			return result;
	}

	bool Body::IsHurt() const
	{
		return std::any_of(bits.begin(), bits.end(), [](const decltype(bits)::value_type& bit)
		{
			return bit.IsHurt();
		});
}

	bool Body::Dead() const
	{
		return (Intelligence().Value() == 0) || (Wisdom().Value() == 0);
	}

	void Body::Disengage()
	{
		for (auto& bit : bits)
		{
			bit.Disengage();
		}
	}

	std::map<const Part*, const Weapon*> Body::Wielded() const
	{
		std::map<const Part*, const Weapon*> result;
		for (auto&  bit : bits)
		{
			if (bit.part.Grip())
				result.emplace(std::make_pair(&bit.part, bit.Held()));
		}
		return result;
	}

	const Weapon* Body::Wielded(const Part& hand) const
	{
		return Find(hand).Held();
	}

	void Body::Hurt(const Part& part, const Damage& damage)
	{
		auto& ouchiebit = Find(part);   // quicker than looking up, functionally equivalent
		ouchiebit.Hurt(damage);
	}

	const Body::Bit& Body::Find(const Part& part) const
	{
		auto it = std::find_if(bits.begin(), bits.end(), [&part](Bit bit)
		{
			return bit.Match(part);
		});
		if (it == bits.end())
			throw std::invalid_argument("Anatomically incorrect body part");
		else
			return *it;
	}

	Body::Bit& Body::Find(const Part& part) 
	{
		auto it = std::find_if(bits.begin(), bits.end(), [&part](Bit bit)
		{
			return bit.Match(part);
		});
		if (it == bits.end())
			throw std::invalid_argument("Anatomically incorrect body part");
		else
			return *it;
	}


	std::set<const Part*> Body::FindAvailable(const Skill& skill) const
	{
		std::set<const Part*> result;
		for (auto& bit : bits)
		{
			if ((bit.IsUsed(skill)) && (bit.IsAvailable(skill)))
				result.emplace(&bit.part);
		}
		return result;
	}

	void Body::Grab(const Part& part, const Weapon& item)
	{
		Find(part).Hold(item);
	}

	void Body::Drop(const Weapon& item)
	{
		for (auto& bit : bits)
		{
			if (bit.Held() == &item)
				bit.Drop();
		}
	}


	std::map<const Part*, const Weapon*> Body::UsedLimbs(const Skill& skill) const
	{
		auto limbs = FindAvailable(skill);

		std::map<const Part*, const Weapon*> result;
		std::transform(limbs.begin(), limbs.end(), std::inserter(result, result.end()), [this](const decltype(limbs)::value_type& limb)
		{
			return std::make_pair(limb, Find(*limb).Held());
		});
		return result;
	}

	std::pair<const Part*, const Weapon*> Body::UsedWeapon(const Skill& skill) const
	{
		auto used = UsedLimbs(skill);
		for (auto use : used)
		{
			if (skill.weapon == Type::Weapon::Style::None)
				return use;
			if ((skill.weapon == Type::Weapon::Style::Unarmed) && (!use.second))
				return use;
			else if ((use.second) && (use.second->Match(skill.weapon)))
				return use;
		}
		return std::pair<const Part*, const Weapon*>(nullptr, nullptr);
	}


	bool Body::Ready(const Skill& skill) const
	{
		auto chain = AvailableKineticChain(skill);
		return !chain.empty();
	}

	std::set<const Part*> Body::AvailableKineticChain(const Skill& skill) const
	{
		if (!anatomy)
			return std::set<const Part*>();

		auto available = FindAvailable(skill);
		std::set<const Part*> result;
		for (auto limb : available)
		{
			auto chain = Anatomical().KineticChain(*limb);
			if (std::all_of(chain.begin(), chain.end(), [this, &skill](const decltype(chain)::value_type& part)
			{
				const auto& bodypart = Find(*part);
				return bodypart.IsAvailable(skill);
			}))
			{
				result.insert(chain.begin(), chain.end());
			}
		}
		return result;
	}


	void Body::Engage(const Skill& skill)
	{
		auto chain = AvailableKineticChain(skill);
		for (auto link : chain)
		{
			Find(*link).Engage(skill);
		}
	}

	Score Body::Strength() const
	{
		return std::accumulate(bits.begin(), bits.end(), Score(), [](const Score& s, const Bit& bit) { return s + bit.AttributeScore(Attribute::Strength); });
	}

	Score Body::Agility() const
	{
		return std::accumulate(bits.begin(), bits.end(), Score(), [](const Score& s, const Bit& bit) { return s + bit.AttributeScore(Attribute::Agility); });
	}

	Score Body::Constitution() const
	{
		return std::accumulate(bits.begin(), bits.end(), Score(), [](const Score& s, const Bit& bit) { return s + bit.AttributeScore(Attribute::Constitution); });
	}

	Score Body::Intelligence() const
	{
		return std::accumulate(bits.begin(), bits.end(), Score(), [](const Score& s, const Bit& bit) { return s + bit.AttributeScore(Attribute::Intelligence); });
	}

	Score Body::Wisdom() const
	{
		return std::accumulate(bits.begin(), bits.end(), Score(), [](const  Score& s, const Bit& bit) { return s + bit.AttributeScore(Attribute::Wisdom); });
	}

	Body& Body::operator=(const Body& state)
	{
		if (anatomy != state.anatomy)
		{	// if anatomy doesn't match (yet), first reinitialize with the same anatomy
			anatomy = state.anatomy;
			Initialize();
		}
		for (const auto& part : anatomy->parts)
		{
			auto& bit = Find(part);
			const auto& otherbit = state.Find(part);
			bit = otherbit;
		}
		return *this;
	}

}
