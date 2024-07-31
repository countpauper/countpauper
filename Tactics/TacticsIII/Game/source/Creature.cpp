#include "Game/Creature.h"
#include "Game/Unarmed.h"

namespace Game
{

Creature::Creature(std::string_view name, const Race& race) :
    Statted({
        {Stat::level, 1},
        {Stat::str, 2},
        {Stat::agi, 2},
        {Stat::con, 2},
        {Stat::wis, 2},
        {Stat::intel, 2}
    }),
    Counted(static_cast<Statted&>(*this)),
    name(name),
    race(race)
{
    InitializeCounters();
    items.push_back(std::make_unique<Unarmed>());
}


std::string_view Creature::Name() const
{
    return name;
}

const Race& Creature::GetRace() const
{
    return race;
}

StatDescriptor Creature::Get(Stat::Id id) const
{
    // Get primary stat
    StatDescriptor result = Statted::Get(id);
    // Get item stat
    if (!result.IsValid())
    {
        for(const auto& item : items)
        {
            result = item->Get(id);
            if (result.IsValid())
            {
                break;
            }
        }
    }
    // Compute secondary stat
    if ((!result.IsValid()) && (id))
    {
        auto it = definition.find(id);
        if (it!=definition.end())
        {
            result = it->second.Compute(*this);
        }
    }

    if (result.IsValid())
    {
        // Add bonuses
        for(const auto& c : conditions)
        {
            result = result.Contribute(c->Name(), c->Contribution(id));
        }
        // TODO: add all other bonuses from items and magic
        result.Contribute(race.Name(), race.Bonus(id));
    }
    return result;
}

const StatDefinition& Creature::Definition() const
{
    return Creature::definition;
}


void Creature::OnCount(Stat::Id stat, unsigned remaining)
{
    if (stat == Stat::hp && remaining == 0)
    {
        Apply<Downed>();
        Apply<KO>();
    }
}

std::vector<std::reference_wrapper<const Condition>> Creature::Conditions() const
{
    std::vector<std::reference_wrapper<const Condition>> result;
    std::transform(conditions.begin(), conditions.end(), std::back_inserter(result),
    [](const decltype(conditions)::value_type& c)
    {
        return std::reference_wrapper<const Condition>(*c);
    });
    return result;
}

StatDefinition Creature::definition;

}
