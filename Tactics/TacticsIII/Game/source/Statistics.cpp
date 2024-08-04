#include "Game/Statistics.h"
#include "Game/StatDefinition.h"

namespace Game
{

Statistics::Statistics(std::initializer_list<std::pair<const Stat::Id, int>> stats) :
    stats(stats)
{
}

StatDescriptor Statistics::Get(Stat::Id id) const
{
    auto it = stats.find(id);
    if (it==stats.end())
    {
        return StatDescriptor();
    }
    StatDescriptor result(Definition().at(id).Limit());
    result.Contribute(Name(), it->second, false);
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

void Statistics::Load(const nlohmann::json& data)
{
    const auto& definition = Definition();
    for(const auto& statDefinition: definition)
    {
        const auto value = Engine::try_get<int>(data, definition.Name(statDefinition.first));
        if (value)
        {
            stats[statDefinition.first] = *value;
        }
    }
}

}
