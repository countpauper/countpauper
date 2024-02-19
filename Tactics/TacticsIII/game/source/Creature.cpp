#include "Game/Creature.h"

namespace Game
{

Creature::Creature(std::string_view name, const Race& race) :
    name(name),
    race(race),
    primaryStats{
        {Stat::level, 1},
        {Stat::str, 2},
        {Stat::agi, 2},
        {Stat::con, 2},
        {Stat::wis, 2},
        {Stat::intel, 2}
    }
{

}

void Creature::Level(Stat::Id stat, int amount)
{
    primaryStats[stat] += amount;
}

std::string_view Creature::Name() const
{
    return name;
}

StatDescriptor Creature::Get(Stat::Id id) const
{
    StatDescriptor result;

    auto it = primaryStats.find(id);
    int base = 0;

    if (it!=primaryStats.end())
    {
        result.Contribute("", it->second, false);
    }
    else
    {
        result = definition[id].Compute(*this);
    }
    // TODO: add all other
    result.Contribute(race.Name(), race.Bonus(id));
    return result;
}

const StatDefinition& Creature::Definition() const
{
    return Creature::definition;
}

StatDefinition Creature::definition;

}
