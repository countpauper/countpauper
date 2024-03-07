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
    for(auto counter: Definition().GetCounters())
    {
        countersUsed[counter] = 0;
    }
}

void Creature::Level(Stat::Id stat, int amount)
{
    primaryStats[stat] += amount;
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

unsigned Creature::CounterAvailable(Stat::Id stat) const
{
    auto counterIt = std::find_if(countersUsed.begin(), countersUsed.end(),[stat, this](const decltype(countersUsed)::value_type& counter)
    {
        return counter.first->ForStat(&Definition().at(stat));
    });
    if (counterIt == countersUsed.end())
       return 0;
    return counterIt->first->Available(counterIt->second, *this);
}

void Creature::Cost(Stat::Id stat, unsigned cost)
{
    auto counterIt = std::find_if(countersUsed.begin(), countersUsed.end(),[stat, this](const decltype(countersUsed)::value_type& counter)
    {
        return counter.first->ForStat(&Definition().at(stat));
    });
    if (counterIt == countersUsed.end())
        return; // throw?

    // range check and clip or throw?
    counterIt->second += cost;
}

void Creature::Reset(Counter::Reset at)
{
    for(auto& counter : countersUsed)
    {
        if (counter.first->ResetWhen(at))
            counter.second = 0;
    }
}


StatDefinition Creature::definition;

}
