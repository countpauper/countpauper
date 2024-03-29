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
    name(name),
    race(race)
{
    for(auto counter: Definition().GetCounters())
    {
        countersUsed[counter] = 0;
    }
    items.push_back(std::make_unique<Unarmed>());
}

void Creature::Level(Stat::Id stat, int amount)
{
    stats[stat] += amount;
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
    StatDescriptor result = Statted::Get(id);

    if (!result)
    {
        for(const auto& item : items)
        {
            result = item->Get(id);
            if (result)
            {
                break;
            }
        }
    }
    if (!result)
    {
        result = definition[id].Compute(*this);
    }
    // TODO: add all other bonuses from items and magic
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
