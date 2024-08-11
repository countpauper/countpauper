#include "Game/StatDefinition.h"
#include <nlohmann/json.hpp>

namespace Game
{
using json = nlohmann::json;

StatDefinition::StatDefinition(std::string_view data)
{
    json parsed = json::parse(data);
    Parse(parsed);
}

void StatDefinition::Parse(const json& parsed)
{
    for (json::const_iterator it = parsed.begin(); it != parsed.end(); ++it)
    {
        const json& stat = it.value();
        auto name = it.key();
        auto id = Stat::Identify(name);
        auto stat_place = emplace(id, Stat(name, stat, *this));
        assert(stat_place.second);  // duplicate state id
        if (auto counterDefinition = Engine::try_get<json>(stat, "counter"))
        {
                counters.emplace_back(id, *counterDefinition, name);
        }
    }
}

Stat::Id StatDefinition::Identify(const Stat* stat) const
{
    if (!stat)
        return Stat::Id::none;
    auto it = std::find_if(begin(), end(), [stat](const value_type& itStat)
    {
        return &itStat.second == stat;
    });
    if (it!=end())
        return it->first;
    else
        throw std::runtime_error(std::string("Stat ") + std::string(stat->Name()) + " not found in the stat definition.");
}

Counter& StatDefinition::Count(Stat::Id id, Counter::Reset reset, bool resetToMax)
{
    counters.emplace_back(Stat::Name(id), id, reset, resetToMax);
    return counters.back();
}

std::vector<const Counter*> StatDefinition::GetCounters() const
{
    std::vector<const Counter*> result;
    result.reserve(counters.size());
    std::transform(counters.begin(), counters.end(), std::back_inserter(result),
        [](const Counter& c)
        {
                return &c;
        });
    return result;
}

Computation StatDefinition::GetPrimaryStat(Stat::Id id) const
{
    auto it = find(id);
    if (it==end())
    {
        return Computation();
    }
    if (!it->second.IsPrimary())
    {
        return Computation();
    }
    return Computation(it->second.Limit());
}

}
