#include "Game/Counters.h"
#include "Game/StatDefinition.h"

namespace Game
{

Counters::Counters(const Statted& stats)  :
    stats(&stats)
{
}

Counters::Counters(const Counters& o) :
    stats(o.stats),
    countersUsed(o.countersUsed)
{
}

Counters::Counters(Counters&& o) :
    stats(nullptr)
{
    std::swap(stats, o.stats);
    std::swap(countersUsed, o.countersUsed);
}

Counters& Counters::operator=(const Counters& o)
{
    stats = o.stats;
    countersUsed = o.countersUsed;
    return *this;
}

Counters& Counters::operator=(Counters&& o)
{
    stats = nullptr;
    countersUsed.clear();
    std::swap(stats, o.stats);
    std::swap(countersUsed, o.countersUsed);
    return *this;
}

void Counters::InitializeCounters()
{   // post constructor because virtual method Definition() is used
    for(auto counter: stats->Definition().GetCounters())
    {
        countersUsed[counter] = 0;
    }
}

unsigned Counters::Available(Stat::Id stat) const
{
    auto counterIt = std::find_if(countersUsed.begin(), countersUsed.end(),[stat, this](const decltype(countersUsed)::value_type& counter)
    {
        return counter.first->ForStat(stat);
    });
    if (counterIt == countersUsed.end())
       return 0;
    return counterIt->first->Available(counterIt->second, *stats);
}

unsigned Counters::Cost(Stat::Id stat, unsigned cost, bool truncate)
{
    auto counterIt = std::find_if(countersUsed.begin(), countersUsed.end(),[stat, this](const decltype(countersUsed)::value_type& counter)
    {
        return counter.first->ForStat(stat);
    });
    if (counterIt == countersUsed.end())
        return 0;
    auto available = counterIt->first->Available(counterIt->second, *stats);
    if (truncate)
        cost = std::min(cost, available);
    else if (cost > available)
        return 0;
    counterIt->second += cost;
    OnCount(stat, available - cost);
    return cost;
}

void Counters::Reset(Counter::Reset at)
{
    for(auto& counter : countersUsed)
    {
        if (counter.first->ResetWhen(at))
        {
            counter.second = 0;
            OnCount(counter.first->MaxStat(), counter.first->Available(0, *stats));
        }
    }
}


void Counters::Deserialize(const json& data)
{
}

json Counters::Serialize() const
{
    auto result = json::object();
    for(const auto& c : countersUsed)
    {
        result[c.first->Name()] = c.second;
    }
    return result;
}

}
