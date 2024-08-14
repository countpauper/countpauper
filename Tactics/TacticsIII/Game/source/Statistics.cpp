#include "Game/Statistics.h"
#include "Game/StatDefinition.h"
#include "Game/Boni.h"

namespace Game
{

Statistics::Statistics(std::initializer_list<std::pair<const Stat::Id, int>> stats) :
    stats(stats)
{
}

Computation Statistics::Get(Stat::Id id, const Boni* extraBoni, const Restrictions&) const
{
    Computation result = Definition().GetPrimaryStat(id);
    auto it = stats.find(id);
    if (it != stats.end())
    {
        result += Computation(it->second, Definition().at(id).Name());
        if (extraBoni)
            result += extraBoni->Bonus(id);
    }
    return result;
}


Computations Statted::Get(std::initializer_list<Stat::Id> stats, const Boni* extraBoni, const Restrictions& restricted) const
{
    Computations result;
    for(auto id: stats)
    {
        result.emplace(std::make_pair(id, Get(id, extraBoni, restricted)));
    }
    return result;
}

void Statistics::Set(Stat::Id id, int value)
{
    stats[id] = value;
}

void Statistics::Level(Stat::Id stat, int amount)
{
    stats[stat] += amount;
}

void Statistics::Deserialize(const nlohmann::json& data)
{
    const auto& definition = Definition();
    stats = definition.Deserialize(data);
}

json Statistics::Serialize() const
{
    return Stat::Serialize(stats);
}

}
