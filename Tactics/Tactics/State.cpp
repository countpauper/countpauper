#include "stdafx.h"
#include <sstream>
#include <numeric>
#include "State.h"
#include "Actor.h"
#include "Action.h"
#include "Skills.h"

namespace Game
{
    State::State(const Actor& actor) :
        position(actor.GetPosition()),
        direction(actor.GetDirection()),
        mp(actor.GetMovePoints()),
        body(actor.body),
        loyalty(actor.GetTeam()),
        worn(actor.Worn()),
        wielded(actor.Wielded()),
        knowledge(actor.GetSkills())
    {
    }

    bool State::IsPossible(const Skill& skill, const State& target) const
    {
        if (mp < skill.mp)
            return false;
        if (position.Distance(target.position) > Range(skill).Value())
            return false;


        // TODO: if weapons.count(skill.weapon)==0 return false
        // TODO if (game.Cover(state.position, target.GetPosition()))
        
        return true;
    }


    Score State::Strength() const
    {
        return body.Strength() + EquipmentBonus(Attribute::Strength); 
    }

    Bonus State::Encumberance() const
    {
        Score encumberance = std::accumulate(worn.begin(), worn.end(), Score(), [](const Score& total, const Armor* armor) -> Score
        {
            int weight = armor->GetLoad().weight;
            return total + Bonus(armor->Name() + L"(" + std::to_wstring(weight) + L"lbs)", weight);
        });
        encumberance = std::accumulate(wielded.begin(), wielded.end(), encumberance, [](const Score& total, const Weapon* weapon) -> Score
        {
            int weight = weapon->GetLoad().weight;
            return total + Bonus(weapon->Name() + L"(" + std::to_wstring(weight) + L"lbs)", weight);
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
        charge = std::accumulate(wielded.begin(), wielded.end(), charge , [](const Score& total, const Weapon* weapon) -> Score
        {
            int enchantment = weapon->GetLoad().enchantment;
            return total + Bonus(weapon->Name() + L"(" + std::to_wstring(enchantment) + L"W)", enchantment);
        });
        auto wisdom = Wisdom();
        charge += Bonus(L"-Wisdom(" + wisdom.Description() + L")x5)", wisdom.Value() * -5);

        return Bonus(L"Charge (" + charge.Description() + L")/5", -static_cast<int>(charge.Value() + 4) / 5);
    }

    Score State::Agility() const
    {
        return body.Agility() + Encumberance() + EquipmentBonus(Attribute::Agility);
    }

    Score State::Constitution() const
    {
        return body.Constitution() + EquipmentBonus(Attribute::Constitution);
    }

    Score State::Intelligence() const
    {
        return body.Intelligence() + Charge() + EquipmentBonus(Attribute::Intelligence);
    }
    Score State::Wisdom() const
    {
        return body.Wisdom() + EquipmentBonus(Attribute::Wisdom);  
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

    Damage State::AttackDamage(const Skill& skill, const Score& skillLevel) const
    {
        auto& attackType = dynamic_cast<Skill::Melee&>(*skill.type);
        auto damageBonus = attackType.DamageBonus(skillLevel);
        auto skillBonus = Bonus(skill.name + L"(" + damageBonus.description + L")", damageBonus.value);
        assert(skillBonus.value >= 0);  // negative skill bonus will be clipped, need it to use in damage operator ^
        auto weapon = MatchWeapon(skill);
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

    Damage State::Mitigation(const Body::Part& location) const
    {
        Score constMitigation(ConstitutionBonus());
        Score wisMitigation(WisdomBonus());
        Damage mitigation = Damage(Wound::Type::Sharp, constMitigation) +
            Damage(Wound::Type::Blunt, constMitigation) +
            Damage(Wound::Type::Burn, constMitigation) +
            Damage(Wound::Type::Disease, constMitigation) +
            Damage(Wound::Type::Spirit, wisMitigation);
        auto slot = location.GetSlot();
        for (auto armor : worn)
        {
            mitigation += armor->Mitigation(slot);
        }
        return mitigation;
    }

    Score State::SkillLevel(const Skill& skill, const State* victim) const
    {
        auto weapon = MatchWeapon(skill);
        Score result = AttributeScore(skill.attribute) + Bonus(skill.name, skill.offset);
        if (victim)
        {
            auto resist = victim->AttributeScore(skill.resist);
            result += Bonus(L"Resist("+resist.Description()+L")", -static_cast<int>(resist.Value()));
        }
        else
        {
            assert(skill.resist == Attribute::None);  // no victim provided for skill with resistance
        }
        result += ArmorBonus(skill);
        if (!weapon)
        {
            return result;
        }
        else
        {
            auto required = weapon->Required();
            auto strength = Strength(); // TODO: strength of applicable hand
            Bonus strengthPenalty = Bonus(L"Str(" + strength.Description()+L")-" +weapon->Name() +L"(" + std::to_wstring(required.strength) + L")",
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

    Score State::EquipmentBonus(Attribute attribute) const
    {
        Score armorBonus = std::accumulate(worn.begin(), worn.end(), Score(), [&attribute](const Score& total, const decltype(worn)::value_type& item)
        {
            return total + item->Bonus(attribute);
        });
        return std::accumulate(wielded.begin(), wielded.end(), armorBonus, [&attribute](const Score& total, const decltype(wielded)::value_type& item)
        {
            return total + item->Bonus(attribute);
        });
    }

    const Weapon* State::MatchWeapon(const Skill& skill) const
    {
        for (auto weapon : wielded)
        {
            if (weapon->Match(skill.weapon))
            {
                return weapon;
            }
        }
        return nullptr;
    }


    Score State::Range(const Skill& skill) const
    {
        Score range(skill.name, skill.range);
        if (auto weapon = MatchWeapon(skill))
        {
            range += weapon->RangeBonus();
        }
        return range;
    }

    GameState::GameState(const IGame& parent) :
        GameState(parent, *parent.Executor())
    {
    }

    GameState::GameState(const IGame& parent, const Actor& executor) :
        parent(parent),
        executor(executor)
    {
    }
    void GameState::Adjust(const Actor& actor, const State& actorState)
    {
        auto insert = state.insert(std::make_pair(&actor, actorState));
        if (!insert.second)
            state.emplace(std::make_pair(&actor,actorState));
    }

    State GameState::Get(const Actor& actor) const
    {
        const auto actorIt = state.find(const_cast<Actor*>(&actor));
        if (actorIt != state.end())
            return actorIt->second;
        return parent.Get(actor);
    }

    void GameState::Apply(IGame& root) const
    {
        parent.Apply(root);

        for (auto& actorState : state)
        {
            root.Adjust(*actorState.first, actorState.second);
        }
    }

    const Actor* GameState::Executor() const
    {
        return &executor;
    }

    State GameState::ActorState() const
    {
        return Get(executor);
    }

    bool GameState::HasParent(const IGame& state) const
    {
        if (&state == this)
            return true;
        if (auto parentState = dynamic_cast<const GameState*>(&parent))
        {
            return parentState->HasParent(state);
        }
        else
        {
            return false;
        }
    }


    bool GameState::CanBe(const Position& position) const
    {
        return parent.CanBe(position);
    }

    bool GameState::CanGo(const Position& from, Direction direction) const
    {
        return parent.CanGo(from, direction);
    }

    bool GameState::Cover(const Position& from, const Position& to) const
    {
        return parent.Cover(from, to);
    }
    std::wstring GameState::Description() const
    {
        std::wstringstream ss;
        for (const auto& actorState : state)
        {
            ss << actorState.first->Description() << L": " << actorState.second.body.Description() << L", "; // L" @ (" << actorState.second.position.Description() << L"), ";
        }
        return ss.str();
    }
} // ::Game
