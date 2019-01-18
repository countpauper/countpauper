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
        knowledge(actor.GetKnowledge())
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
        return body.Strength();  // todo: equipment boni
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
            int weight = armor->GetLoad().enchantment;
            return total + Bonus(armor->Name() + L"(" + std::to_wstring(weight) + L"W)", weight);
        });
        charge = std::accumulate(wielded.begin(), wielded.end(), charge , [](const Score& total, const Weapon* weapon) -> Score
        {
            int weight = weapon->GetLoad().enchantment;
            return total + Bonus(weapon->Name() + L"(" + std::to_wstring(weight) + L"W)", weight);
        });
        auto wisdom = Wisdom();
        charge += Bonus(L"-Wisdom(" + wisdom.Description() + L")x5)", wisdom.Value() * -5);

        return Bonus(L"Charge (" + charge.Description() + L")/5", -static_cast<int>(charge.Value() + 4) / 5);
    }

    Score State::Agility() const
    {
        return body.Agility() + Encumberance();
    }

    Score State::Constitution() const
    {
        return body.Constitution();  // todo: equipment boni
    }

    Score State::Intelligence() const
    {
        return body.Intelligence() + Charge();
    }
    Score State::Wisdom() const
    {
        return body.Wisdom();  // todo:equipment boni
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
        auto weapon = MatchWeapon(skill);

        if (!weapon)
        {
            return Damage(Wound::Type::Blunt, Score(L"Unarmed", 2) + StrengthBonus());
        }
        else
        {
            auto strengthBonus = StrengthBonus();
            auto wisdomBonus = WisdomBonus();
            auto skillLevel = SkillLevel(skill);
            auto& attackType = dynamic_cast<Skill::Melee&>(*skill.type);
            auto skillBonus = Bonus(attackType.DamageBonus(skillLevel));
            return weapon->Damage() ^ (
                Damage(Wound::Type::Blunt, Score(strengthBonus) + skillBonus) +
                Damage(Wound::Type::Sharp, Score(strengthBonus) + skillBonus) +
                Damage(Wound::Type::Burn, Score(wisdomBonus) + skillBonus) +
                Damage(Wound::Type::Disease, Score(wisdomBonus) + skillBonus) +
                Damage(Wound::Type::Spirit, Score(wisdomBonus) + skillBonus));
        }
    }

    Score State::Chance(const Skill& skill) const
    {
        auto skillLevel = SkillLevel(skill);
        auto skillBonus = skill.GetChance(skillLevel);
        Score chance(skillBonus);
        if (skill.trigger == Skill::Trigger::Defend)
        {
            auto agility = Agility();

            chance += Bonus(L"Agi(" + agility.Description() + L")", (int(agility.Value() * 3)));
            if (auto weapon = MatchWeapon(skill))
            {
                chance += weapon->DefenseBonus();
            }

            return chance;
        }
        return chance;
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

    Score State::SkillLevel(const Skill& skill) const
    {
        auto it = std::find_if(knowledge.begin(), knowledge.end(),
            [&skill](const Actor::Knowledge::value_type& known)
        {
            return known.skill == &skill;
        });
        if (it == knowledge.end())
            return Score();
        else
            return Score(Bonus(skill.name, it->score));
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
