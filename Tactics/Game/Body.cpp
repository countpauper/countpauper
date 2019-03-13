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
    Body::Part::Part() :
        slot(Slot::Nothing),
        engagement(nullptr),
        held(nullptr),
        control(nullptr)
    {
    }

    Body::Part::Part(std::wstring name, Anatomy anatomy, Slot slot, const Stats& stats, const Damage& health) :
        name(name),
        anatomy(anatomy),
        slot(slot),
        stats(stats),
        health(health),
        engagement(nullptr),
        held(nullptr),
        control(nullptr)
    {


    }
    bool Body::Part::operator<(const Part& other) const
    {
        return name < other.name;
    }
    
    bool Body::Part::Match(Anatomy match) const
    {
        return anatomy.Contains(match);
    }

    bool Body::Part::Disabled() const
    {
        return health.Disabled();
    }

    void Body::Part::Hurt(const Damage& damage)
    {
        health += damage;
    }

    bool Body::Part::IsVital() const
    {
        // TODO: flag
        return Contributes(Attribute::Intelligence) || 
            Contributes(Attribute::Wisdom);
    }

    bool Body::Part::Grip() const
    {
        // TODO: flag
        return Contributes(Attribute::Strength);
    }

    std::wstring Body::Part::Name() const
    {
        return name;
    }
    std::wstring Body::Part::Description() const
    {
        return name + L"=" + health.StateDescription();
    }

    Score Body::Part::AttributeScore(Attribute attribute) const
    {
        auto value = stats[attribute];
        if (value != 0)
            return Score(name, value) + Bonus(health.StateDescription(), health.StatPenalty());
        else
            return Score();
    }

    bool Body::Part::IsHurt() const
    {
        return health.Hurt();
    }

    Slot Body::Part::GetSlot() const
    {
        return slot;
    }

    unsigned Body::Part::Height() const
    {
        return anatomy.position + anatomy.size;
    }

    unsigned Body::Part::Length() const
    {
        return anatomy.size;
    }

    bool Body::Part::Contributes(Attribute attribute) const
    {
        return stats[attribute] > 0;
    }

    void Body::Part::Engage(const Skill& skill)
    {
        engagement = &skill;
    }

    void Body::Part::Disengage()
    {
        engagement = nullptr;
    }

    void Body::Part::Hold(const Weapon& item)
    {
        if (!Grip())
            throw std::runtime_error("Body part can't grip item");
        held = &item;
    }

    const Weapon* Body::Part::Held() const
    {
        return held;
    }
    void Body::Part::Drop()
    {
        held = nullptr;
    }

    bool Body::Part::IsAvailable(const Skill& skill) const
    {
        if (control)
            return false;
        if ((engagement) && (!skill.Combo(*engagement)))
            return false;
        if (Grip())
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

    bool Body::Part::IsUsed(const Skill& skill) const
    {
        if (Grip())
        {
            if (skill.Require(held))
                return true;
            else if ((held == nullptr) && (Contributes(skill.attribute)))
                return true;
            else
                return false;
        }
        else if (skill.weapon == Type::Weapon::None)
        {
            return Contributes(skill.attribute);
        }
        return false;
    }

    std::wstring Body::Description() const
    {
        std::wstring result;
        bool fine = true;
        for (auto& part : parts)
        {
            if (part.IsHurt())
            {
                if (!fine)
                    result += L",";
                result += part.Description();
                fine = false;
            }
        }
        if (fine)
            return L"Healthy";
        else
            return result;
    }

    bool Body::Dead() const
    {
        return (Intelligence().Value() == 0) || (Wisdom().Value() == 0);
    }

    void Body::Disengage()
    {
        for (auto& part : parts)
        {
            part.Disengage();
        }
    }

    std::map<const Body::Part*, const Weapon*> Body::Wielded() const
    {
        std::map<const Body::Part*, const Weapon*> result;
        for (auto& part : parts)
        {
            if (part.Grip())
                result.emplace(std::make_pair(&part, part.Held()));
        }
        return result;
    }

    void Body::Hurt(const Part& part, const Damage& damage)
    {
        auto& ouchiepart = const_cast<Part&>(part);   // quicker than looking up, functionally equivalent
        ouchiepart.Hurt(damage);
    }

    const Body::Part* Body::Get(const Anatomy& location) const
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

    const Body::Part* Body::Get(const std::wstring& name) const
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

    Body::Part& Body::operator[](const std::wstring& name)
    {
        for (auto& part : parts)
        {
            if (part.Name() == name)
            {
                return part;
            }
        }
        throw std::out_of_range("Unknown body part");
    }

    Body::Part& Body::operator[](const Part& part)
    {
        return const_cast<Part&>(*Get(part.Name()));
    }

    std::set<const Body::Part*> Body::Grip() const
    {
        std::set<const Body::Part*> result;
        for (auto& part : parts)
        {
            if (part.Grip())
                result.emplace(&part);
        }
        return result;
    }

    std::set<const Body::Part*> Body::FindAvailable(const Skill& skill) const
    {
        std::set<const Body::Part*> result;
        for (auto& part : parts)
        {
            if ((part.IsUsed(skill)) && (part.IsAvailable(skill)))
                result.emplace(&part);
        }
        return result;
    }

	std::map<const Body::Part*, const Weapon*> Body::UsedLimbs(const Skill& skill) const
	{
		auto limbs = FindAvailable(skill);

		std::map<const Body::Part*, const Weapon*> result;
		std::transform(limbs.begin(), limbs.end(), std::inserter(result, result.end()), [](const decltype(limbs)::value_type& limb)
		{
			return std::make_pair(limb, limb->Held());
		});
		return result;
	}

	std::pair<const Body::Part*, const Weapon*> Body::UsedWeapon(const Skill& skill) const
	{
		auto used = UsedLimbs(skill);
		for (auto use : used)
		{
			if ((skill.weapon == Type::Weapon::None) && (!use.second))
				return use;
			else if ((use.second) && (use.second->Match(skill.weapon)))
				return use;
		}
		return std::pair<const Body::Part*, const Weapon*>(nullptr, nullptr);
	}


    bool Body::Ready(const Skill& skill) const
    {
        auto chain = AvailableKineticChain(skill);
        return !chain.empty();
    }

    std::set<const Body::Part*> Body::AvailableKineticChain(const Skill& skill) const
    {
        auto available = FindAvailable(skill);
        std::set<const Body::Part*> result;
        for (auto limb : available)
        {
            auto chain = KineticChain(*limb);
            if (std::all_of(chain.begin(), chain.end(), [&skill](const decltype(chain)::value_type& part)
            {
                return part->IsAvailable(skill);
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
            (*this)[*link].Engage(skill);
        }
   }


    std::set<const Body::Part*> Body::KineticChain(const Part& origin) const
    {
        // Doesn't actually connect the whole chain, just adds a (first) agility body part
        std::set<const Body::Part*> result;
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

    unsigned Body::Length() const
    {
        return std::accumulate(parts.begin(), parts.end(), 0U, [](unsigned max, const decltype(parts)::value_type& part)
        {
            return std::max(max, part.Height());
        });
    }

    Damage Body::InnateDamage() const
    {
        // TODO: configure limb used and use its current "strength" for bite/tail attacks  
        return Damage(Wound::Type::Blunt, Score(L"Innate",1));
    }
    Score Body::Strength() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const Score& s, const Part& part) { return s + part.AttributeScore(Attribute::Strength); });
    }

    Score Body::Agility() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const Score& s, const Part& part) { return s + part.AttributeScore(Attribute::Agility); });
    }

    Score Body::Constitution() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const Score& s, const Part& part) { return s + part.AttributeScore(Attribute::Constitution); });
    }
    
    Score Body::Intelligence() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const Score& s, const Part& part) { return s + part.AttributeScore(Attribute::Intelligence); });
    }
    
    Score Body::Wisdom() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const  Score& s, const Part& part) { return s + part.AttributeScore(Attribute::Wisdom); });
    }

    void Body::Add(const Part& p)
    {
        parts.emplace_back(p);
    }

    std::wistream& operator>>(std::wistream& s, Body::Part& part)
    {
        Attributes attributes;
        Stats::Score score;
        s >> part.name >> part.anatomy >> part.slot >> attributes >> score >> part.health;
        part.stats = Stats(attributes, score);

        return s;
    }

    std::wistream& operator>>(std::wistream& s, Body& body)
    {
        unsigned parts;
        s >> parts;
        body.parts.resize(parts);
        for (unsigned i = 0; i < parts; ++i)
        {
            s >> body.parts[i];
        }
        return s;
    }

}
