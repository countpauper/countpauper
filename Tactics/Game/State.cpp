#include "stdafx.h"
#include <sstream>
#include <numeric>
#include "State.h"
#include "Actor.h"
#include "Action.h"
#include "Skills.h"

namespace Game
{
	State::State() :
		mp(0),
		direction(Direction::None),
		loyalty(0)
	{
	}

	State::State(const Actor& actor) :
        position(actor.GetPosition()),
        direction(actor.GetDirection()),
        mp(actor.GetMovePoints()),
        body(actor.body),
        loyalty(actor.GetTeam()),
        worn(actor.Worn()),
        carried(actor.Carried()),
        knowledge(actor.GetSkills())
    {
    }

	Position State::GetPosition() const
	{
		return position;
	}
	
	std::wstring State::Description() const
	{
		return std::wstring(L"State [") + std::to_wstring(unsigned(this))+L"]";
	}

    State::State(const Body& body, Position pos, const Direction dir, unsigned mp,
        std::vector<const Armor*> armor, std::vector<const Weapon*> weapons, Actor::Knowledge skills) :
		position(pos),
        direction(dir),
        mp(mp),
        body(body),
        loyalty(0),
        worn(armor),
        carried(weapons),
        knowledge(skills)
    {
    }


    bool State::IsPossible(const Skill& skill, const Target& target) const
    {
        if (mp < skill.mp)
            return false;
        auto available = body.UsedWeapon(skill);
        if (!available.first)
            return false;
        if (position.DistanceEl(target.GetPosition()) > Range(skill).Value())
            return false;
        if (body.UsedLimbs(skill).empty())
            return false;
        // TODO if (game.Cover(state.position, target.GetPosition()))
        
        return true;
    }


    Score State::Strength() const
    {
        return body.Strength() + AttributeBonus(Attribute::Strength);
    }

    Score State::AttributeScore(const Body::Part& limb, Attribute attribute) const
    {
        // private, because limb has to be part of the state's body, which is copied
        auto wielded = body.Wielded();
		const auto weapon = wielded[&limb];
        auto result = limb.AttributeScore(attribute);
        auto body = FullBodyBonus(attribute);
        assert(body.Value() % 2 == 0);  // TODO: rounding
        result += Bonus(body.Description(), body.Value() / wielded.size()); 
        if (weapon)
        {
            result += weapon->Bonus(attribute); 
        }
		// TODO: divide encumberance and charge over unused limbs
		if (attribute == Attribute::Agility)
		{
			result += Encumberance();
		}
		else if (attribute == Attribute::Intelligence)
		{
			result += Charge();
		}
        return result;
    }

    Bonus State::Encumberance() const
    {
        Score encumberance = std::accumulate(worn.begin(), worn.end(), Score(), [](const Score& total, const Armor* armor) -> Score
        {
            int weight = armor->GetLoad().weight;
            return total + Bonus(armor->Name() + L"(" + std::to_wstring(weight) + L"lbs)", weight);
        });
        encumberance = std::accumulate(carried.begin(), carried.end(), encumberance, [](const Score& total, const decltype(carried)::value_type& weapon) -> Score
        {
            if (weapon)
            {
                int weight = weapon->GetLoad().weight;
                return total + Bonus(weapon->Name() + L"(" + std::to_wstring(weight) + L"lbs)", weight);
            }
            else
                return total;
        });
        auto strength = Strength();
        encumberance += Bonus(L"-Strength (" + strength.Description() + L")x5)", strength.Value() * -5);

        return Bonus(L"Encumberance ("+encumberance.Description()+L")/5", -static_cast<int>(encumberance.Value()+4)/5);
    }

    Bonus State::Charge() const
    {
        Score charge = std::accumulate(worn.begin(), worn.end(), Score(), [](const Score& total, const Armor* armor) -> Score
        {
            int enchantment = armor->GetLoad().enchantment;
            return total + Bonus(armor->Name() + L"(" + std::to_wstring(enchantment) + L"W)", enchantment);
        });
        charge = std::accumulate(carried.begin(), carried.end(), charge , [](const Score& total, const decltype(carried)::value_type& weapon) -> Score
        {
            if (weapon)
            {
                int enchantment = weapon->GetLoad().enchantment;
                return total + Bonus(weapon->Name() + L"(" + std::to_wstring(enchantment) + L"W)", enchantment);
            }
            else
                return total;
        });
        auto wisdom = Wisdom();
        charge += Bonus(L"-Wisdom(" + wisdom.Description() + L")x5W)", wisdom.Value() * -5);

        return Bonus(L"Charge (" + charge.Description() + L")/5", -static_cast<int>(charge.Value() + 4) / 5);
    }

    Score State::Agility() const
    {
        return body.Agility() + Encumberance() + AttributeBonus(Attribute::Agility);
    }

    Score State::Constitution() const
    {
        return body.Constitution() + AttributeBonus(Attribute::Constitution);
    }

    Score State::Intelligence() const
    {
        return body.Intelligence() + Charge() + AttributeBonus(Attribute::Intelligence);
    }
    Score State::Wisdom() const
    {
        return body.Wisdom() + AttributeBonus(Attribute::Wisdom);
    }

    Score State::AttributeScore(Attribute attribute) const
    {
        Score attrScore;
        switch (attribute)
        {
        case Attribute::Strength:
            attrScore = Strength();
            break;
        case Attribute::Agility:
            attrScore = Agility();
            break;
        case Attribute::Constitution:
            attrScore = Constitution();
            break;
        case Attribute::Intelligence:
            attrScore = Intelligence();
            break;
        case Attribute::Wisdom:
            attrScore = Wisdom();
            break;
        case Attribute::None:
        default:
            return Score();
        }
        return Score(ToString(attribute) + L"(" + attrScore.Description() + L")", attrScore.Value());
    }

    Bonus State::ConstitutionBonus() const
    {
        auto consitution = Constitution();
        return Bonus(L"Con (" + consitution.Description() + L")", (int(consitution.Value()) - 11) / 3);
    }

    Bonus State::StrengthBonus() const
    {
        auto strength = Strength();
        return Bonus(L"Str (" + strength.Description() + L")", (int(strength.Value()) - 11) / 3);
    }

    Bonus State::IntelligenceBonus() const
    {
        auto intelligence = Intelligence();
        return Bonus(L"Int (" + intelligence.Description() + L")", (int(intelligence.Value()) - 11) / 3);
    }

    Bonus State::WisdomBonus() const
    {
        auto wisdom = Wisdom();
        return Bonus(L"Wis (" + wisdom.Description() + L")", (int(wisdom.Value()) - 11) / 3);
    }

    Damage State::AttackDamage(const Skill& skill) const
    {
        return AttackDamage(skill, SkillLevel(skill));
    }

    Damage State::AttackDamage(const Skill& skill, const Score& skillLevel) const
    {
        auto& attackType = dynamic_cast<Skill::Melee&>(*skill.type);
        auto damageBonus = attackType.DamageBonus(skillLevel);
        auto skillBonus = Bonus(skill.name + L"(" + damageBonus.description + L")", damageBonus.value);
        assert(skillBonus.value >= 0);  // negative skill bonus will be clipped, need it to use in damage operator ^
        auto weapon = body.UsedWeapon(skill).second;

        Damage skillDamage =
            Damage(Wound::Type::Blunt, Score(skillBonus)) +
            Damage(Wound::Type::Sharp, Score(skillBonus)) +
            Damage(Wound::Type::Burn, Score(skillBonus)) +
            Damage(Wound::Type::Disease, Score(skillBonus)) +
            Damage(Wound::Type::Spirit, Score(skillBonus));
        if (weapon)
        {
            return weapon->Damage() ^ skillDamage;
        }
        else
        {
            return body.InnateDamage()^ skillDamage;
        }
    }

	bool State::Hurt(const Body::Part& part, const Damage& damage, const std::wstring& description)
    {
        Damage pain = damage - Mitigation(part);
		if (!pain.Hurt())
			return false;
        body.Hurt(part, pain.Wound(description));
        if (body.Dead())
            direction.Fall();

        return true;
    }

	void State::Spent(unsigned spentMP)
	{
		if (spentMP > mp)
			throw std::runtime_error("Impossible move cost");
		mp -= spentMP;
	}

    Score State::Chance(const Skill& skill, const State& target) const
    {
        auto skillLevel = SkillLevel(skill, &target);
        auto skillBonus = skill.GetChance(skillLevel);
        return Score(skillBonus);
    }

    Score State::Chance(const Skill& skill) const
    {
        auto skillLevel = SkillLevel(skill);
        auto skillBonus = skill.GetChance(skillLevel);
        return Score(skillBonus);
    }

    Damage State::Mitigation(const Body::Part& part) const
    {
        Score constMitigation(ConstitutionBonus());
        Score wisMitigation(WisdomBonus());
        Damage mitigation = Damage(Wound::Type::Sharp, constMitigation) +
            Damage(Wound::Type::Blunt, constMitigation) +
            Damage(Wound::Type::Burn, constMitigation) +
            Damage(Wound::Type::Disease, constMitigation) +
            Damage(Wound::Type::Spirit, wisMitigation);
        auto slot = part.GetSlot();
        for (auto armor : worn)
        {
            mitigation += armor->Mitigation(slot);
        }
        return mitigation;
    }

    Anatomy State::Origin(const Skill& skill, Trajectory trajectory) const
    {
        auto wield = body.UsedWeapon(skill);
        auto limb = wield.first;
        if (!limb)
        {
            auto available = body.FindAvailable(skill);
            if (available.empty())
                throw std::runtime_error("No skill origin because no limb is available"); // should not be possible
            limb = *available.begin();
        }
        if (limb)
        {
            auto height = limb->Height();
            return Anatomy(trajectory, height, 3);  // TODO: reach for height
        }
        else if (skill.attribute == Attribute::None)
        {
            return Anatomy(Plane::Front, body.Length()/2,1);
        }
        else
        {
            return Anatomy();
        }
    }

    Score State::SkillLevel(const Skill& skill, const State* victim) const
    {
        if (knowledge.count(&skill) == 0)
            return Score();

        auto used = body.UsedLimbs(skill);

        Score result(Bonus(skill.name, skill.offset));
        for (auto use : used)
        {
            result += AttributeScore(*use.first, skill.attribute);
        }
        if (victim)
        {
            auto resist = victim->AttributeScore(skill.resist);
            result += Bonus(L"Resist("+resist.Description()+L")", -static_cast<int>(resist.Value()));
        }
        
        result += ArmorBonus(skill);
        auto weapon = body.UsedWeapon(skill).second;
        if (!weapon)
        {
            return result;
        }
        else
        {
            auto strength = Score();
            for (auto use : used)
            {
                strength += AttributeScore(*use.first, Attribute::Strength);
            }
            auto required = weapon->Required();
            Bonus strengthPenalty = Bonus(L"Str(" + strength.Description() + L")-" + weapon->Name() + L"(" + std::to_wstring(required.strength) + L")",
                std::min(0, static_cast<int>(strength.Value()) - static_cast<int>(required.strength)));
            auto intelligence = Intelligence();
            Bonus intPenalty = Bonus(L"Int(" + intelligence.Description() +L")-" +weapon->Name() + L"(" + std::to_wstring(required.intelligence) + L")",
                std::min(0, static_cast<int>(intelligence.Value()) - static_cast<int>(required.intelligence)));
            auto weaponBonus = weapon->Bonus(skill);
            return result + strengthPenalty + intPenalty + weaponBonus;
        }
    }

    Score State::ArmorBonus(const Skill& skill) const
    {
        return std::accumulate(worn.begin(), worn.end(), Score(), [&skill](const Score& total, const decltype(worn)::value_type& item)
        {
            return total + item->Bonus(skill);
        });
    }

    Score State::FullBodyBonus(Attribute attribute) const
    {
        Score armorBonus = std::accumulate(worn.begin(), worn.end(), Score(), [&attribute](const Score& total, const decltype(worn)::value_type& item)
        {
            return total + item->Bonus(attribute);
        });
        // TODO: effect bonus
        return armorBonus;
    }

    Score State::AttributeBonus(Attribute attribute) const
    {
        Score bonus = FullBodyBonus(attribute);
        auto wielded = body.Wielded();
        return std::accumulate(wielded.begin(), wielded.end(), bonus, [&attribute](const Score& total, const decltype(wielded)::value_type& wield)
        {
            if (wield.second)
                return total + wield.second->Bonus(attribute);
            else
                return Score(total);
        });
    }

    Score State::Range(const Skill& skill) const
    {
        Score range(skill.name, skill.range);
        auto wield = body.UsedWeapon(skill);
        if (wield.second)
        {
            range += Bonus(wield.second->Name(), wield.second->Length());
            range += Bonus(wield.first->Name(), wield.first->Length());
        }
        return range;
    }

    void State::Engage(const Skill& skill)
    {
        body.Engage(skill);
    }
} // ::Game
